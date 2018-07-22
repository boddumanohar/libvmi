#ifndef BAREFLANK_EVENTS_PRIVATE_H
#define BAREFLANK_EVENTS_PRIVATE_H

//#include <sys/poll.h>
#include <unistd.h>
#include<libmvi/events.h>

typedef struct {
    //xc_evtchn* xce_handle;
    int port;
    uint32_t evtchn_port;
    void *ring_page;
    union {
        vm_event_46_back_ring_t back_ring_46;
        vm_event_48_back_ring_t back_ring_48;
    };
    uint64_t max_gpfn;
    uint32_t monitor_capabilities;
    bool monitor_singlestep_on;
    bool monitor_mem_access_on;
    bool monitor_intr_on;
    bool monitor_cr0_on;
    bool monitor_cr3_on;
    bool monitor_cr4_on;
    bool monitor_xcr0_on;
    bool monitor_msr_on;
} bareflank_vm_event_t;

/* Conversion matrix from LibVMI flags to Xen vm_event flags */
static const unsigned int event_response_conversion[] = {
    [VMI_EVENT_RESPONSE_EMULATE] = VM_EVENT_FLAG_EMULATE,
    [VMI_EVENT_RESPONSE_EMULATE_NOWRITE] = VM_EVENT_FLAG_EMULATE_NOWRITE,
    [VMI_EVENT_RESPONSE_TOGGLE_SINGLESTEP] = VM_EVENT_FLAG_TOGGLE_SINGLESTEP,
    [VMI_EVENT_RESPONSE_SET_EMUL_READ_DATA] = VM_EVENT_FLAG_SET_EMUL_READ_DATA,
    [VMI_EVENT_RESPONSE_DENY] = VM_EVENT_FLAG_DENY,
    [VMI_EVENT_RESPONSE_VMM_PAGETABLE_ID] = VM_EVENT_FLAG_ALTERNATE_P2M,
    [VMI_EVENT_RESPONSE_SET_REGISTERS] = VM_EVENT_FLAG_SET_REGISTERS,
    [VMI_EVENT_RESPONSE_SET_EMUL_INSN] = VM_EVENT_FLAG_SET_EMUL_INSN_DATA,
    [VMI_EVENT_RESPONSE_GET_NEXT_INTERRUPT] = VM_EVENT_FLAG_GET_NEXT_INTERRUPT,
};

typedef struct bareflank_events {
    union {
        //xen_mem_event_t mem_event;
        bareflank_vm_event_t vm_event;
    };
} bareflank_events_t;

typedef struct bareflank_instance bareflank_instance_t;
status_t wait_for_event_or_timeout(bareflank_instance_t *bareflank, /*xc_evtchn *xce,*/ unsigned long ms);

#endif
