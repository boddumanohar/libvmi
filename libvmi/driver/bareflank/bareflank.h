#ifndef BAREFLANK_DRIVER_H
#define BAREFLANK_DRIVER_H

#include "libvmi.h"

status_t bareflank_init(
    vmi_instance_t vmi,
    uint32_t init_flags,
    void *init_data);
status_t bareflank_init_vmi(
    vmi_instance_t vmi,
    uint32_t init_flags,
    void *init_data);
void bareflank_destroy(
    vmi_instance_t vmi);

static inline status_t
driver_bareflank_setup(vmi_instance_t vmi)
{
    driver_interface_t driver = { 0 };
    driver.initialized = true;
    driver.init_ptr = &bareflank_init;
   	driver.init_vmi_ptr = &bareflank_init_vmi;
		driver.destroy_ptr = &bareflank_destroy; 
    vmi->driver = driver;
    return VMI_SUCCESS;
}

#endif
