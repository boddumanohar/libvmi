#ifndef BAREFLANK_PRIVATE_H
#define BAREFLANK_PRIVATE_H

#define _GNU_SOURCE

#include "private.h"

typedef struct bareflank_instance {

    //char *name; no concept of name for now

    uint64_t vcpuid; /**< VM that we are accessing */

    //int major_version;  /**< Major version of Bareflank LibMVI is running on */

    //int minor_version;  /**< Minor version of Bareflank LibMVI is running on */

    vm_type_t type; /**< VM type (HVM/PV32/PV64) */

} bareflank_instance_t;

static inline
bareflank_instance_t *bareflank_get_instance(
    vmi_instance_t vmi)
{
    return ((bareflank_instance_t *) vmi->driver.driver_data);
}

#endif /* BAREFLANK_PRIVATE_H */
