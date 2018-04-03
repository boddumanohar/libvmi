#ifndef BAREFLANK_PRIVATE_H
#define BAREFLANK_PRIVATE_H

#define _GNU_SOURCE

/*
#define XC_WANT_COMPAT_EVTCHN_API 1
#define XC_WANT_COMPAT_MAP_FOREIGN_API 1
*/

#include <fnmatch.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/*
#include <xenctrl.h>
#include <xen/hvm/save.h>

#include "private.h"
#include "libxc_wrapper.h"
#include "libxs_wrapper.h"
#include "driver/xen/xen_events_private.h"
*/

typedef struct bareflank_instance {

    char *name;

//  xc_interface* xchandle; /**< handle to xenctrl library (libxc) */

//  struct xs_handle *xshandle;  /**< handle to xenstore daemon (libxs) */

//  libxc_wrapper_t libxcw; /**< wrapper for libxc for cross-compatibility */

//  libxs_wrapper_t libxsw; /**< wrapper for libxs for cross-compatibility */

    uint64_t vcpuid; /**< VM that we are accessing */

    int major_version;  /**< Major version of Bareflank LibMVI is running on */

    int minor_version;  /**< Minor version of Bareflank LibMVI is running on */

    vm_type_t type; /**< VM type (HVM/PV32/PV64) */

//  xc_dominfo_t info;      /**< libxc info: domid, ssidref, stats, etc */

    xen_events_t *events; /**< handle to events data */

    uint64_t max_gpfn;    /**< result of xc_domain_maximum_gpfn/2() */

} bareflank_instance_t;

static inline
bareflank_instance_t *bareflank_get_instance(
    vmi_instance_t vmi)
{
    return ((xen_instance_t *) vmi->driver.driver_data);
}

/*
static inline
xc_interface* xen_get_xchandle(
    vmi_instance_t vmi)
{
    return xen_get_instance(vmi)->xchandle;
}
*/

/*
static inline xen_events_t*
xen_get_events(vmi_instance_t vmi)
{
    return xen_get_instance(vmi)->events;
}
*/

#endif /* XEN_PRIVATE_H */
