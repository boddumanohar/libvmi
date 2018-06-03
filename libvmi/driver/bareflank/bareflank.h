#ifndef BAREFLANK_DRIVER_H
#define BAREFLANK_DRIVER_H

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
void *bareflank_read_page(
    vmi_instance_t vmi,
    addr_t page);
status_t bareflank_get_memsize(
    vmi_instance_t vmi,
    uint64_t *allocated_ram_size,
    addr_t *maximum_physical_address);
status_t bareflank_get_vcpuregs(
    vmi_instance_t vmi,
    registers_t *regs,
    unsigned long vcpu);
status_t bareflank_get_vcpureg(
		vmi_instance_t vmi,
		uint64_t *value,
		reg_t reg,
		unsigned long vcpu);
status_t bareflank_set_vcpureg(
    vmi_instance_t vmi,
    uint64_t value,
    reg_t reg,
    unsigned long vcpu);
status_t bareflank_set_vcpuregs(
    vmi_instance_t vmi,
    registers_t *regs,
    unsigned long vcpu);

static inline status_t
driver_bareflank_setup(vmi_instance_t vmi)
{
    driver_interface_t driver = { 0 };
    driver.initialized = true;
    driver.init_ptr = &bareflank_init;
   	driver.init_vmi_ptr = &bareflank_init_vmi;
		driver.destroy_ptr = &bareflank_destroy; 
		//driver.get_id_from_name_ptr = &xen_get_domainid_from_name;
    //driver.get_name_from_id_ptr = &xen_get_name_from_domainid;
    //driver.get_id_ptr = &xen_get_domainid;
    //driver.set_id_ptr = &xen_set_domainid;
    //driver.check_id_ptr = &xen_check_domainid;
    //driver.get_name_ptr = &xen_get_domainname;
    //driver.set_name_ptr = &xen_set_domainname;
    driver.get_memsize_ptr = &bareflank_get_memsize;
		driver.get_vcpureg_ptr = &bareflank_get_vcpureg;
    driver.get_vcpuregs_ptr = &bareflank_get_vcpuregs;
		driver.set_vcpureg_ptr = &bareflank_set_vcpureg;
    driver.set_vcpuregs_ptr = &bareflank_set_vcpuregs;
		driver.read_page_ptr = &bareflank_read_page;
		
		//driver.write_ptr = &xen_write;
    //driver.is_pv_ptr = &xen_is_pv; will not be implemented
    //driver.pause_vm_ptr = &xen_pause_vm;
    //driver.resume_vm_ptr = &xen_resume_vm;
    //driver.set_access_required_ptr = &xen_set_access_required;	
    vmi->driver = driver;
    return VMI_SUCCESS;
}

#endif
