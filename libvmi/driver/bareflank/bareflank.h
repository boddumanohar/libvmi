#ifndef BAREFLANK_DRIVER_H
#define BAREFLANK_DRIVER_H

status_t bareflank_get_memsize(
	  vmi_instance_t vmi,
    uint64_t *allocate_ram_size,
    addr_t *maximum_physical_address);
status_t bareflank_test(
		uint64_t domainid,
		const char *name);
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
status_t bareflank_get_vcpuregs(
    vmi_instance_t vmi,
    registers_t *regs,
    unsigned long vcpu);
status_t bareflank_get_vcpureg(
		vmi_instance_t vmi,
		uint64_t *value,
		reg_t reg,
		unsigned long vcpu);

static inline status_t
driver_bareflank_setup(vmi_instance_t vmi)
{
    driver_interface_t driver = { 0 };
    driver.initialized = true;
    driver.init_ptr = &bareflank_init;
   	driver.init_vmi_ptr = &bareflank_init_vmi;
		driver.destroy_ptr = &bareflank_destroy; 
    driver.get_vcpuregs_ptr = &bareflank_get_vcpuregs;
		driver.get_vcpureg_ptr = &bareflank_get_vcpureg;
    vmi->driver = driver;
    return VMI_SUCCESS;
}

#endif
