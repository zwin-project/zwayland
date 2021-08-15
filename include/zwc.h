#ifndef ZWC_H
#define ZWC_H

struct zwc_display;

struct zwc_display* zwc_display_create(const char* name);

void zwc_display_destroy(struct zwc_display* display);

int zwc_display_prepare_read(struct zwc_display* display);

int zwc_display_dispatch_pending(struct zwc_display* display);

int zwc_display_flush(struct zwc_display* display);

int zwc_display_read_events(struct zwc_display* display);

int zwc_display_get_fd(struct zwc_display* display);

#endif  //  ZWC_H
