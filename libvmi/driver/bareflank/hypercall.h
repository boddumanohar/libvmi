#ifndef BAREFLANK_HYPERCALL_H
#define BAREFLANK_HYPERCALL_H

#include "libvmi.h"
status_t get_type_info();

status_t get_current_vcpu_id();

// just tells whether bareflank is reachable or not
long int get_bareflank_status();

#endif
