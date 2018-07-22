#ifndef BAREFLANK_EVENTS_H
#define BAREFLANK_EVENTS_H

status_t bareflank_init_events(
    vmi_instance_t vmi,
    uint32_t init_flags,
    void *init_data);

void bareflank_events_destroy(vmi_instance_t vmi);

#endif
