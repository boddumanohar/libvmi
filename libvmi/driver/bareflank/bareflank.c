#define _GNU_SOURCE

#include <stdlib.h>
#include <json-c/json.h>

#include "private.h"
#include "driver/driver_wrapper.h"
#include "driver/memory_cache.h"
#include "driver/bareflank/bareflank.h"
#include "driver/bareflank/bareflank_private.h"

#include <sched.h>
#include <unistd.h>

typedef enum hcall{ // to check hypercall status
	HCALL_SUCCESS,
	HCALL_FAILURE
}hcall_t;

// Bareflank libvmi ABI:
// Use eax to send vmcall ID
// use rdi to send address of buffer
// use rsi to send buffer size

typedef struct json_object json_object;

status_t bareflank_set_vcpureg(vmi_instance_t vmi, uint64_t value, reg_t reg, unsigned long vcpu)
{
	//set cpu affinity

	char input[80];
	switch(reg){
		case RAX:
			sprintf(input, "{\"RAX\":\"%ld\"}", value);	
	case RBX:
			sprintf(input, "{\"RBX\":\"%ld\"}", value);	
	case RCX:
				sprintf(input, "{\"RCX\":\"%ld\"}", value);	
	case RDX:
				sprintf(input, "{\"RDX\":\"%ld\"}", value);	
	case RSI:
				sprintf(input, "{\"RSI\":\"%ld\"}", value);	
	case RDI:
				sprintf(input, "{\"RDI\":\"%ld\"}", value);	
	case RIP:
				sprintf(input, "{\"RIP\":\"%ld\"}", value);	
	}

}
status_t bareflank_set_vcpuregs(vmi_instance_t vmi, registers_t *regs, unsigned long vcpu)
{	

}

hcall_t h_get_vcpuregs(unsigned long vcpu, json_object **jobj)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	if (vcpu < 4) {
		CPU_SET(vcpu,&mask);
		size_t cpusetsize = sizeof(mask);
		if(-1 == sched_setaffinity(0,cpusetsize,&mask)){
			errprint("Could not set cpu affinity\n");
			return HCALL_FAILURE;
		}
	}

	hcall_t ret = HCALL_SUCCESS;
	size_t size = 4096; // 1 page // TODO:replace 4096 with system page size
	void *buffer = malloc(size); // TODO: is malloc the best way to alloc mem to buffer?
	
	asm("movq %0, %%rdi"
		  :
      :"a"(buffer)
      :"rdi"
      );

  asm("movq %0, %%rsi"
      :
      :"a"(size)
      :"rsi"
      );

	

	asm("mov $2, %eax");
	asm("vmcall");

	//TODO: set cpu affinity to normal mode

	uint64_t status;	
	asm( "movq %%rdx, %0" // using rdx to return hypercall status
      : "=a" (status)
      );

	//TODO:
	//if (status != 1){
		// some went wrong during hypercall
	//	return ret;
	//}

	*jobj = json_tokener_parse((char *)buffer);
	free(buffer);
	return ret;
}

static uint64_t parse_reg_value(const char *reg, json_object *root)
{
	// parse the json and get the value of the key
	
	json_object *return_obj = NULL;
	json_object_object_get_ex(root,reg,&return_obj);
	return json_object_get_int64(return_obj);

}

static status_t getkeyfrom_json(json_object *root, reg_t reg, uint64_t *value) {

	status_t ret = VMI_SUCCESS;
	//TODO: get segment registers
	switch (reg) {
					case CR0:
							*value = parse_reg_value("CR0", root);
							break;
					case CR2:
							*value = parse_reg_value("CR2", root);
							break;
					case CR3:
							*value = parse_reg_value("CR3", root);
							break;
					case CR4:
							*value = parse_reg_value("CR4", root);
							break;
					case DR0:
							*value = parse_reg_value("DR0", root);
							break;
					case DR1:
							*value = parse_reg_value("DR1", root);
							break;
					case DR2:
							*value = parse_reg_value("DR2", root);
							break;
					case DR3:
							*value = parse_reg_value("DR3", root);
							break;
					case DR6:
							*value = parse_reg_value("DR6", root);
							break;
					case DR7:
							*value = parse_reg_value("DR7", root);
							break;
						case RAX:
								*value = parse_reg_value("RAX", root);
								break;
						case RBX:
								*value = parse_reg_value("RBX", root);
								break;
						case RCX:
								*value = parse_reg_value("RCX", root);
								break;
						case RDX:
								*value = parse_reg_value("RDX", root);
								break;
						case RBP:
								*value = parse_reg_value("RBP", root);
								break;
						case RSI:
								*value = parse_reg_value("RSI", root);
								break;
						case RDI:
								*value = parse_reg_value("RDI", root);
								break;
						case RSP:
								*value = parse_reg_value("RSP", root);
								break;
						case R8:
								*value = parse_reg_value("R08", root);
								break;
						case R9:
								*value = parse_reg_value("R09", root);
								break;
						case R10:
								*value = parse_reg_value("R10", root);
								break;
						case R11:
								*value = parse_reg_value("R11", root);
								break;
						case R12:
								*value = parse_reg_value("R12", root);
								break;
						case R13:
								*value = parse_reg_value("R13", root);
								break;
						case R14:
								*value = parse_reg_value("R14", root);
								break;
						case R15:
								*value = parse_reg_value("R15", root);
								break;
						case RIP:
								*value = parse_reg_value("RIP", root);
								break;
						case RFLAGS:
								*value = parse_reg_value("RFL", root);
								break;
						case MSR_EFER:
								*value = parse_reg_value("MSR_EFER", root);
								break;
						default:
								ret = VMI_FAILURE;
								break;
							//}
						//} 

			}
	return ret;
	
}



status_t bareflank_get_vcpureg(
		vmi_instance_t vmi,
		uint64_t *value,
		reg_t reg,
		unsigned long vcpu)
{
	status_t ret = VMI_SUCCESS;
	
	json_object *j_regs = NULL;
	if (HCALL_FAILURE == h_get_vcpuregs(vcpu, &j_regs))	

	if (j_regs == NULL) {
		errprint("json object is null\n");
		return VMI_FAILURE;
	}

	if (VMI_SUCCESS != getkeyfrom_json(j_regs, reg, value))
			return VMI_FAILURE;

	return ret; 
}

status_t
bareflank_get_vcpuregs(
    vmi_instance_t vmi,
    registers_t *regs,
    unsigned long vcpu)
{
	status_t ret = VMI_SUCCESS;

  json_object *j_regs = NULL;
	if (HCALL_FAILURE == h_get_vcpuregs(vcpu, &j_regs))	{
		errprint("failed to registers of of VCPU %lu \n", vcpu);
		return VMI_FAILURE;
	} 

	if (j_regs == NULL) 
		errprint("json object is null\n");

    regs->x86.rax = parse_reg_value("RAX",j_regs);
    regs->x86.rbx = parse_reg_value("RBX",j_regs);
    regs->x86.rcx = parse_reg_value("RCX",j_regs);
    regs->x86.rdx = parse_reg_value("RDX",j_regs);
    regs->x86.rbp = parse_reg_value("RBP",j_regs);
    regs->x86.rsi = parse_reg_value("RSI",j_regs);
    regs->x86.rdi = parse_reg_value("RDI",j_regs);
    regs->x86.rsp = parse_reg_value("RSP",j_regs);
    regs->x86.r8  = parse_reg_value("R08",j_regs);
    regs->x86.r9  = parse_reg_value("R09",j_regs);
    regs->x86.r10 = parse_reg_value("R10",j_regs);
    regs->x86.r11 = parse_reg_value("R11",j_regs);
    regs->x86.r12 = parse_reg_value("R12",j_regs);
    regs->x86.r13 = parse_reg_value("R13",j_regs);
    regs->x86.r14 = parse_reg_value("R14",j_regs);
    regs->x86.r15 = parse_reg_value("R15",j_regs);
    regs->x86.rip = parse_reg_value("RIP",j_regs);
    regs->x86.cr0 = parse_reg_value("CR0",j_regs);
    regs->x86.cr2 = parse_reg_value("CR2",j_regs);
    regs->x86.cr3 = parse_reg_value("CR3",j_regs);
    regs->x86.cr4 = parse_reg_value("CR4",j_regs);
    regs->x86.dr7 = parse_reg_value("RIP",j_regs);

		return ret;
}

status_t
bareflank_get_memsize(
    vmi_instance_t vmi,
    uint64_t *allocated_ram_size,
    addr_t *maximum_physical_address)
{
    
    *allocated_ram_size = 3941696 * 1024; // convert KBytes to bytes
    
		*maximum_physical_address = *allocated_ram_size;

    return VMI_SUCCESS;
}

void *
bareflank_read_page(
    vmi_instance_t vmi,
    addr_t page)
{
		//errprint("inside bareflank read page\n");
    addr_t paddr = page << vmi->page_shift;

    return memory_cache_insert(vmi, paddr);
}

void
bareflank_destroy(
    vmi_instance_t vmi)
{
    bareflank_instance_t *bareflank = bareflank_get_instance(vmi);
		if (!bareflank) return;

		// normally xen & kvm just destroy the handle to the library. since we
		// don't have a library we just free the memory allocated to that pointer.

    g_free(bareflank);

    vmi->driver.driver_data = NULL;
}

hcall_t h_get_bareflank_status() {
	asm("movl $1, %eax");
	asm("vmcall"); // TODO:if VMM is absent, the program crashes saying illegal instruction
	return HCALL_SUCCESS; 
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

		errprint("\n");

    if ( HCALL_SUCCESS  != h_get_bareflank_status() ) {
				g_free(bareflank);
        return VMI_FAILURE;
		}

		dbprint(VMI_DEBUG_DRIVER, "running in bareflank");

		vmi->driver.driver_data = (void*)bareflank;

    return VMI_SUCCESS;
}

// Here we get take the domainid and domain-name and validate
// whether they are correct or not. 

status_t
bareflank_test(uint64_t domainid, const char *name)
{
    struct vmi_instance _vmi = {0};
    vmi_instance_t vmi = &_vmi;

    if ( VMI_FAILURE == bareflank_init(vmi, 0, NULL) )
        return VMI_FAILURE;

		bareflank_destroy(vmi);
    return VMI_SUCCESS;
}


void h1_map_foreign_range(void *buffer, size_t size, size_t page_size, int prot, unsigned long pfn){
	// TODO: Functions for mapping foreign domain's memory
		asm("movq %0, %%rdi"
		  :
      :"a"(buffer)
      :"rdi"
      );

  /*asm("movq %0, %%rsi"
      :
      :"a"(size)
      :"rsi"
      );*/

	asm("movq %0, %%rbx"
      :
      :"a"(pfn)
      :"rbx"
      );

/*asm("movq %0, %%rcx"
      :
      :"a"(page_size)
      :"rsi"
      );*/



	errprint("going to mmap hypercall\n");
	asm("mov $4, %eax");
	asm("vmcall");

			errprint("%s \n", (char*)buffer);
	//TODO: set cpu affinity to normal mode

	/*asm( "movq %%rdx, %0" // using rdx to return hypercall status
      : "=a" (buffer)
      );
  */
	
	//return buffer;
}
void h_lookup(addr_t vaddr, addr_t *paddr){
	
				asm("movq %0, %%rdi"
					:
					:"a"(vaddr)
					:"rdi"
				 );
	asm("mov $6, %eax");
	asm("vmcall");

	      asm( "movq %%rdx, %0" // using rdx to return hypercall status
	      : "=a" (*paddr)
	      );

	return;

}

void *
bareflank_get_memory_pfn(
    vmi_instance_t vmi,
    addr_t pfn,
    int prot)
{
			void *buffer = memset(malloc(4096*3),0, 4096*3);
			addr_t vaddr = buffer;
			addr_t paddr;
			// get start vaddr of the next page
			vaddr = (vaddr - 0x160) + 0x1000; // vaddr - size_of_malloc_header + 4096
			h_lookup(vaddr, &paddr);

			// now paddr is the start of a fresh page
			errprint("vaddr %p - gpa %p \n", vaddr, paddr);

				asm("movq %0, %%rdi"
					:
					:"a"(paddr)
					:"rdi"
				 );

			asm("movq %0, %%rbx"
					:
					:"a"((unsigned long)pfn)
					:"rbx"
				 );

			asm("mov $5, %eax");
			asm("vmcall");

			void *outbuf = vaddr;
			void *newbuf = malloc(4096);
			memcpy(newbuf, outbuf, 4096); // 4096/8
		if (NULL == buffer) {
			dbprint(VMI_DEBUG_XEN, "--bareflank_get_memory_pfn failed on pfn=0x%"PRIx64"\n", pfn);
			return NULL;
    } else {
        dbprint(VMI_DEBUG_XEN, "--bareflank_get_memory_pfn success on pfn=0x%"PRIx64"\n", pfn);
    }

    return (void *)newbuf;
}

void *
bareflank_get_memory(
    vmi_instance_t vmi,
    addr_t paddr,
    uint32_t UNUSED(length))
{
    addr_t pfn = paddr >> vmi->page_shift;

		//errprint("for paddr %ld the pfn is %ld \n", paddr, pfn);
    return bareflank_get_memory_pfn(vmi, pfn, PROT_READ);
}

void
bareflank_release_memory(
    void *memory,
    size_t length)
{
    //munmap(memory, length);
		
		free(memory);
		errprint("done with release memory\n");
		return;
}
/** 
 * Setup Bareflank live mode.
 */
status_t
bareflank_setup_live_mode(
    vmi_instance_t vmi)
{
    dbprint(VMI_DEBUG_XEN, "--Bareflank: setup live mode\n");
    memory_cache_destroy(vmi);
    memory_cache_init(vmi, bareflank_get_memory, bareflank_release_memory, 0);
    return VMI_SUCCESS;
}
status_t
bareflank_init_vmi(
    vmi_instance_t vmi,
    uint32_t UNUSED(init_flags),
    void *UNUSED(init_data))
{
    status_t ret = VMI_SUCCESS;
    bareflank_instance_t *bareflank = bareflank_get_instance(vmi);
		// Each Bareflank VM uses only 1 vcpu

		//TODO: initialize the fields of bareflank instance
    ret = bareflank_setup_live_mode(vmi);
		if (VMI_FAILURE == ret)
			return VMI_FAILURE;

		return ret;	
}

