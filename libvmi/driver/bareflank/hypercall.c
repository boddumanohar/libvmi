#include "hypercall.h"

uint32_t get_current_vcpu_id() {

// read the value of cpuid from the r1 register.

}

// returns VMI_FAILURE or VMI_SUCCESS

long int get_bareflank_status() {

asm("vmcall");
asm("movl $1, %eax");
register unsigned long i asm("eax");

}

uint32_t get_type_info() {

}
