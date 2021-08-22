#ifndef ZWC_DISPLAY_H
#define ZWC_DISPLAY_H

struct zwc_display {
  struct wl_display* wl_display;
  struct zwc_global* global;
};

#endif  //  ZWC_DISPLAY_H
