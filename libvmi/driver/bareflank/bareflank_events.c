#include<string.h>
#include "private.h"
#include "driver/bareflank/bareflank.h"
#include "driver/bareflank/bareflank_private.h"
#include "driver/bareflank/bareflank_events.h"
#include "driver/bareflank/bareflank_events_private.h"

status_t wait_for_event_or_timeout(xen_instance_t *xen, xc_evtchn *xce, unsigned long ms)
{
    struct pollfd fd = {
        .fd = xen->libxcw.xc_evtchn_fd(xce),
        .events = POLLIN | POLLERR
    };

    switch ( poll(&fd, 1, ms) ) {
        case -1:
            if (errno == EINTR)
                return VMI_SUCCESS;

            errprint("Poll exited with an error\n");
            return VMI_FAILURE;
        case 0:
            return VMI_SUCCESS;
        default: {
            int port = xen->libxcw.xc_evtchn_pending(xce);
            if ( -1 == port ) {
                errprint("Failed to read port from event channel\n");
                return VMI_FAILURE;
            }

            if ( xen->libxcw.xc_evtchn_unmask(xce, port) ) {
                errprint("Failed to unmask event channel port\n");
                return VMI_FAILURE;
            }

            return VMI_SUCCESS;
        }
    };

    return VMI_FAILURE;
}

void get_request(bareflank_vm_event_t *mem_event,
                 vm_event_46_request_t *req)
{
    vm_event_46_back_ring_t *back_ring;
    RING_IDX req_cons;

    back_ring = &mem_event->back_ring_46;
    req_cons = back_ring->req_cons;

    // Copy request
    memcpy(req, RING_GET_REQUEST(back_ring, req_cons), sizeof(*req));
    req_cons++;

    // Update ring
    back_ring->req_cons = req_cons;
    back_ring->sring->req_event = req_cons + 1;
}

static inline
void put_response(bareflank_vm_event_t *mem_event,
                  vm_event_46_response_t *rsp)
{
    vm_event_46_back_ring_t *back_ring;
    RING_IDX rsp_prod;

    back_ring = &mem_event->back_ring_46;
    rsp_prod = back_ring->rsp_prod_pvt;

    // Copy response
    memcpy(RING_GET_RESPONSE(back_ring, rsp_prod), rsp, sizeof(*rsp));
    rsp_prod++;
    
    // Update ring
    back_ring->rsp_prod_pvt = rsp_prod;
    RING_PUSH_RESPONSES(back_ring);
}

static inline
void process_response ( event_response_t response, vmi_event_t* event, vm_event_46_request_t *rsp )
{
    if ( response && event ) {
        uint32_t i = VMI_EVENT_RESPONSE_NONE+1;

        for (; i<=__VMI_EVENT_RESPONSE_MAX; i++) {
            event_response_t er = 1u << i;

            if ( response & er ) {
                switch ( er ) {
                    case VMI_EVENT_RESPONSE_VMM_PAGETABLE_ID:
                        rsp->altp2m_idx = event->slat_id;
                        break;
                    case VMI_EVENT_RESPONSE_SET_EMUL_READ_DATA:
                        if ( event->emul_read ) {
                            rsp->flags |= event_response_conversion[VMI_EVENT_RESPONSE_EMULATE];

                            if ( event->emul_read->size < sizeof(rsp->data.emul_read_data.data) )
                                rsp->data.emul_read_data.size = event->emul_read->size;
                            else
                                rsp->data.emul_read_data.size = sizeof(rsp->data.emul_read_data.data);

                            memcpy(&rsp->data.emul_read_data.data,
                                   &event->emul_read->data,
                                   rsp->data.emul_read_data.size);

                            if ( !event->emul_read->dont_free )
                                free(event->emul_read);
                        }
                        break;
                    case VMI_EVENT_RESPONSE_SET_REGISTERS:
                        memcpy(&rsp->data.regs.x86, event->x86_regs, sizeof(struct regs_x86));
                        break;
                };

                rsp->flags |= event_response_conversion[er];
            }
        }
    }
}

static inline
status_t process_register(vmi_instance_t vmi,
                          reg_t reg,
                          vm_event_46_request_t *req,
                          vm_event_46_request_t *rsp)
{
    gint lookup = reg;
    vmi_event_t * event = g_hash_table_lookup(vmi->reg_events, &lookup);

    if ( !event )
        return VMI_FAILURE;

    switch ( reg ) {
        case MSR_ALL:
            event->reg_event.msr = req->u.mov_to_msr.msr;
            event->reg_event.value = req->u.mov_to_msr.value;
            break;
        case CR0:
        case CR3:
        case CR4:
        case XCR0:
            /*
 *              * event->reg_event.equal allows for setting a reg event for
 *                           *  a specific VALUE of the register
 *                                        */
            if ( event->reg_event.equal &&
                    event->reg_event.equal != req->u.write_ctrlreg.new_value )
                return VMI_SUCCESS;

            event->reg_event.value = req->u.write_ctrlreg.new_value;
            event->reg_event.previous = req->u.write_ctrlreg.old_value;
            break;
        default:
            break;
    }

    event->vcpu_id = req->vcpu_id;
    event->x86_regs = (x86_registers_t *)&req->data.regs.x86;

    vmi->event_callback = 1;
    process_response ( event->callback(vmi, event), event, rsp );
    vmi->event_callback = 0;

    return VMI_SUCCESS;
}

static inline
status_t process_requests(vmi_instance_t vmi, vm_event_46_request_t *req,
                          vm_event_46_response_t *rsp)
{
    bareflank_events_t * xe = bareflank_get_events(vmi);
    status_t vrc = VMI_SUCCESS;

    while ( RING_HAS_UNCONSUMED_REQUESTS(&xe->vm_event.back_ring_46) ) {
        memset( req, 0, sizeof (vm_event_46_request_t) );
        memset( rsp, 0, sizeof (vm_event_46_response_t) );

        get_request(&xe->vm_event, req);

        if ( req->version != 0x00000001 ) {
            errprint("Error, Xen reports a VM_EVENT_INTERFACE_VERSION that doesn't match what we expected (0x00000001)!\n");
            return VMI_FAILURE;
        }

        rsp->version = req->version;
        rsp->vcpu_id = req->vcpu_id;
        rsp->flags = (req->flags & VM_EVENT_FLAG_VCPU_PAUSED);
        rsp->reason = req->reason;

        /*
         * When we shut down we pull all pending requests from the ring
         */

        if ( vmi->shutting_down ) {
            if ( req->reason == VM_EVENT_REASON_MEM_ACCESS )
                rsp->u.mem_access.gfn = req->u.mem_access.gfn;
        } else
            switch ( req->reason ) {
                case VM_EVENT_REASON_MEM_ACCESS:
                    dbprint(VMI_DEBUG_XEN, "--Caught mem access event!\n");
                    vrc = process_mem(vmi, req, rsp);
                    break;

                case VM_EVENT_REASON_WRITE_CTRLREG:
                    switch ( req->u.write_ctrlreg.index ) {
                        case VM_EVENT_X86_CR0:
                            dbprint(VMI_DEBUG_XEN, "--Caught MOV-TO-CR0 event!\n");
                            vrc = process_register(vmi, CR0, req, rsp);
                            break;

                        case VM_EVENT_X86_CR3:
                            dbprint(VMI_DEBUG_XEN, "--Caught MOV-TO-CR3 event!\n");
                            vrc = process_register(vmi, CR3, req, rsp);
                            break;

                        case VM_EVENT_X86_CR4:
                            dbprint(VMI_DEBUG_XEN, "--Caught MOV-TO-CR4 event!\n");
                            vrc = process_register(vmi, CR4, req, rsp);
                            break;

                        case VM_EVENT_X86_XCR0:
                            dbprint(VMI_DEBUG_XEN, "--Caught MOV-TO-XCR0 event!\n");
                            vrc = process_register(vmi, XCR0, req, rsp);
                            break;
                        default:
                            dbprint(VMI_DEBUG_XEN, "--Caught unknown WRITE_CTRLREG event!\n");
                            break;

                    }
                    break;

                case VM_EVENT_REASON_MOV_TO_MSR:
                    dbprint(VMI_DEBUG_XEN, "--Caught MOV-TO-MSR event!\n");
                    vrc = process_register(vmi, MSR_ALL, req, rsp);
                    break;

                case VM_EVENT_REASON_SINGLESTEP:
                    dbprint(VMI_DEBUG_XEN, "--Caught single step event!\n");
                    vrc = process_single_step_event(vmi, req, rsp);
                    break;

                case VM_EVENT_REASON_SOFTWARE_BREAKPOINT:
                    dbprint(VMI_DEBUG_XEN, "--Caught int3 interrupt event!\n");
                    vrc = process_interrupt_event(vmi, INT3, req, rsp);
                    break;

                case VM_EVENT_REASON_GUEST_REQUEST:
                    dbprint(VMI_DEBUG_XEN, "--Caught guest requested event!\n");
                    vrc = process_guest_requested_event(vmi, req, rsp);
                    break;

                default:
                    errprint("UNKNOWN REASON CODE %d\n", req->reason);
                    vrc = VMI_FAILURE;
                    break;
            }

        /*
 *          * Put the response on the ring
 *                   */
        put_response(&xe->vm_event, rsp);
        dbprint(VMI_DEBUG_XEN, "--Finished handling event.\n");
    }

    return vrc;
}
