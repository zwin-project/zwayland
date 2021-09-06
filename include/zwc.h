#ifndef ZWC_H
#define ZWC_H

/* zwc display */

struct zwc_display;

struct zwc_display* zwc_display_create(const char* name);

void zwc_display_destroy(struct zwc_display* display);

int zwc_display_prepare_read(struct zwc_display* display);

int zwc_display_dispatch_pending(struct zwc_display* display);

int zwc_display_flush(struct zwc_display* display);

int zwc_display_read_events(struct zwc_display* display);

int zwc_display_get_fd(struct zwc_display* display);

/* zwc virtual object */

struct zwc_virtual_object;

struct zwc_virtual_object* zwc_virtual_object_create(struct zwc_display* display);

void zwc_virtual_object_destroy(struct zwc_virtual_object* virtual_object);

void zwc_virtual_object_commit(struct zwc_virtual_object* virtual_object);

typedef void (*zwc_virtual_object_frame_callback_done_func_t)(void* data, uint32_t callback_time);

void zwc_virtual_object_add_frame_callback(struct zwc_virtual_object* virtual_object,
                                           zwc_virtual_object_frame_callback_done_func_t done_func,
                                           void* data);

void zwc_virtual_object_attach_surface(struct zwc_virtual_object* virtual_object, int32_t width,
                                       int32_t height, uint32_t size, uint32_t format, uint8_t* data);

#endif  //  ZWC_H
