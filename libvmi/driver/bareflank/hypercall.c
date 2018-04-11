#include "hypercall.h"

uint32_t get_current_vcpu_id() {

_asm("mov $0 %r1;
vmcall;
")

// read the value of cpuid from the r1 register.

}

// returns VMI_FAILURE or VMI_SUCCESS

status_t get_bareflank_status() {


}

uint32_t get_type_info() {

}
