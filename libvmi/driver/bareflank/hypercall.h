// Action: get current VM type info

// identify which vm to go and give vmcall to that vm.
// return the type of guest PV or HVM
// OUT
// 1 - HVM guest
// 2 - PV guest
uint32_t get_type_info();

uint32_t get_current_vcpu_id();

// just tells whether bareflank is reachable or not
status_t get_bareflank_status();
