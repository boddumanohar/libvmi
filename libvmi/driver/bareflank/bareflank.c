#include "driver/driver_interface.h"
#include "driver/memory_cache.h"
#include "driver/bareflank/hypercall.h"
#include "driver/bareflank/bareflank.h"
#include "driver/bareflank/bareflank_private.h"
void
bareflank_destroy(
    vmi_instance_t vmi)
{
    barelfank_instance_t *bareflank = bareflank_get_instance(vmi);
		if (!bareflank) return;

		// normally xen & kvm just destroy the handle to the library. since we
		// don't have a library we just free the memory allocated to that pointer.

    //g_free(bareflank->name);
    g_free(bareflank);

    vmi->driver.driver_data = NULL;
}

status_t
bareflank_init(
    vmi_instance_t vmi,
    uint32_t UNUSED(init_flags),
    void *UNUSED(init_data))
{
    bareflank_instance_t *bareflank = g_malloc0(sizeof(bareflank_instance_t));

		// since bareflank accepts VMCALL from user space on Intel based systems,
		// we don't need to create an wrapper. So, here we just get bareflank status.
    if ( 1  != get_bareflank_status(bareflank) )
        return VMI_FAILURE;

		dbprint(VMI_DEBUG_DRIVER, "running in bareflank");

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

		// Each Bareflank VM uses only 1 vcpu

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

