#include "../../driver/driver_interface.h"
#include "../../driver/memory_cache.h"

void
bareflank_destroy(
    vmi_instance_t vmi)
{
    barelfank_instance_t *bareflank = bareflank_get_instance(vmi);
		if (!bareflank) return;
		//destroy_domain_socket(bareflank);
		dlclose(bareflank->handle);
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

		uint32_t id = get_current_vcpu_id();	
		bareflank->vcpuid = id;
    vmi->driver.driver_data = (void*)bareflank;

    return VMI_SUCCESS;
}

// Here we get take the domainid and domain-name and validate
// whether they are correct or not. 

status_t
kvm_test()
{
    struct vmi_instance _vmi = {0};
    vmi_instance_t vmi = &_vmi;

    if ( VMI_FAILURE == bareflank_init(vmi, 0, NULL) )
        return VMI_FAILURE;

		// TODO: Best way to way to validate a VM in bareflank. 

    /*if (name) {
        domainid = bareflank_get_id_from_name(vmi, name);
        if (domainid != VMI_INVALID_DOMID)
            return VMI_SUCCESS;
    }*/

    /*if (domainid != VMI_INVALID_DOMID) {
        char *_name = NULL;
        status_t rc = kvm_get_name_from_id(vmi, domainid, &_name);
        free(_name);
		*/
        if ( VMI_SUCCESS == rc )
            return rc;

    bareflank_destroy(vmi);
    return VMI_FAILURE;
}
