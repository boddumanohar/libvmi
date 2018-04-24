#include "../../driver/driver_interface.h"
#include "../../driver/memory_cache.h"

void
bareflank_destroy(
    vmi_instance_t vmi)
{
    barelfank_instance_t *bareflank = bareflank_get_instance(vmi);
		if (!bareflank) return;

		// normally xen & kvm just destroy the handle to the library.
		// so we just free the memory allocated to that pointer.
		free(bareflank);
}

//----------------------------------------------------------------------------
// General Interface Functions (1-1 mapping to driver_* function)

status_t
bareflank_init(
    vmi_instance_t vmi,
    uint32_t UNUSED(init_flags),
    void *UNUSED(init_data))
{
    bareflank_instance_t *bareflank = g_malloc0(sizeof(bareflank_instance_t));
		// size bareflank accepts VMCALL from user space on Intel based systems,
		// we don't need to create an wrapper.
		// So, here we just fetch status bareflank. If we are able to get some
		// information, then we add it to driver.driver_data otherwise we return
		// VMI_FAILURE 
    if ( VMI_FAILURE == get_bareflank_status(bareflank) )
        return VMI_FAILURE;

		vmi->driver.driver_data = (void*)bareflank;

    return VMI_SUCCESS;
}

// Here we get take the domainid and domain-name and validate
// whether they are correct or not. 

status_t
bareflank_test()
{
    struct vmi_instance _vmi = {0};
    vmi_instance_t vmi = &_vmi;

    if ( VMI_FAILURE == bareflank_init(vmi, 0, NULL) )
        return VMI_FAILURE;

		// TODO: Best way to way to validate a VM in bareflank. 

		bareflank_destroy(vmi);
    return VMI_SUCCESS;
}

status_t
bareflank_init_vmi(
    vmi_instance_t vmi,
    uint32_t init_flags,
    void *init_data)
{
    status_t ret = VMI_FAILURE;
    bareflank_instance_t *bareflank = bareflank_get_instance(vmi);
    int rc;

    /* TODO: record the count of VCPUs used by this instance and possibly
			 add it to the bareflank instance or we may create a info struct 
			 which has all the details of the VM */

		/* for now, bareflank only uses only 1 vcpu */

		// initialize the fields of bareflank instance
		int ret = get_current_vcpu_id();
		if (ret > 0) {
			bareflank->vcpuid = ret
		}
		else {
			goto _bail;	
		}

		int ret = get_type_info();
		if( ret > 0) {
			bareflank->type = ret;
		}
		else {
			goto _bail;	
		}

_bail:
    return ret;
}

void
bareflank_destroy(
    vmi_instance_t vmi)
{
    bareflank_instance_t *bareflank = bareflank_get_instance(vmi);

    if (!bareflank) return;

		/*TODO: Destroy events if type is HVM */

    g_free(bareflank->name);
    g_free(bareflank);

    vmi->driver.driver_data = NULL;
}

static status_t
bareflank_get_vcpuregs_hvm(
    vmi_instance_t vmi,
    registers_t *regs,
    unsigned long vcpu)
{
    xen_instance_t *bareflank = bareflank_get_instance(vmi);
    struct hvm_hw_cpu hw_ctxt = {0}, *hvm_cpu = NULL;

    if (NULL == hvm_cpu) {
        if (xen->libxcw.xc_domain_hvm_getcontext_partial(xen->xchandle,
                xen->domainid,
                HVM_SAVE_CODE(CPU),
                vcpu,
                &hw_ctxt,
                sizeof hw_ctxt)) {
            errprint("Failed to get context information (HVM domain).\n");
            return VMI_FAILURE;
        }
        hvm_cpu = &hw_ctxt;
    }

    regs->x86.rax = hvm_cpu->rax;
    regs->x86.rbx = hvm_cpu->rbx;
    regs->x86.rcx = hvm_cpu->rcx;
    regs->x86.rdx = hvm_cpu->rdx;
    regs->x86.rbp = hvm_cpu->rbp;
    regs->x86.rsi = hvm_cpu->rsi;
    regs->x86.rdi = hvm_cpu->rdi;
    regs->x86.rsp = hvm_cpu->rsp;
    regs->x86.r8 = hvm_cpu->r8;
    regs->x86.r9 = hvm_cpu->r9;
    regs->x86.r10 = hvm_cpu->r10;
    regs->x86.r11 = hvm_cpu->r11;
    regs->x86.r12 = hvm_cpu->r12;
    regs->x86.r13 = hvm_cpu->r13;
    regs->x86.r14 = hvm_cpu->r14;
    regs->x86.r15 = hvm_cpu->r15;
    regs->x86.rip = hvm_cpu->rip;
    regs->x86.rflags = hvm_cpu->rflags;
    regs->x86.cr0 = hvm_cpu->cr0;
    regs->x86.cr2 = hvm_cpu->cr2;
    regs->x86.cr3 = hvm_cpu->cr3;
    regs->x86.cr4 = hvm_cpu->cr4;
    regs->x86.dr7 = hvm_cpu->dr7;
    regs->x86.fs_base = hvm_cpu->fs_base;
    regs->x86.gs_base = hvm_cpu->gs_base;
    regs->x86.cs_arbytes = hvm_cpu->cs_arbytes;
    regs->x86.sysenter_cs = hvm_cpu->sysenter_cs;
    regs->x86.sysenter_esp = hvm_cpu->sysenter_esp;
    regs->x86.sysenter_eip = hvm_cpu->sysenter_eip;
    regs->x86.msr_efer = hvm_cpu->msr_efer;
    regs->x86.msr_star = hvm_cpu->msr_star;
    regs->x86.msr_lstar = hvm_cpu->msr_lstar;

    return VMI_SUCCESS;
}

status_t
xen_get_vcpuregs(
    vmi_instance_t vmi,
    registers_t *regs,
    unsigned long vcpu)
{
#if defined(I386) || defined (X86_64)
    if (vmi->vm_type == HVM)
        return xen_get_vcpuregs_hvm(vmi, regs, vcpu);
#endif

    return VMI_FAILURE;
}
