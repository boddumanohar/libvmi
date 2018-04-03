#ifndef BAREFLANK_DRIVER_H
#define BAREFLANK_DRIVER_H

static inline status_t
driver_xen_setup(vmi_instance_t vmi)
{
    driver_interface_t driver = { 0 };
    driver.initialized = true;
    //driver.init_ptr = &xen_init;
   	//driver.init_vmi_ptr = &xen_init_vmi;
		//driver.destroy_ptr = &xen_destroy; 
    vmi->driver = driver;
    return VMI_SUCCESS;
}
