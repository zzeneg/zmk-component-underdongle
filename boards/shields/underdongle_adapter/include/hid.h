#pragma once

#include <zmk/event_manager.h>

#ifdef CONFIG_RAW_HID

struct is_connected_notification {
    bool value;
};

ZMK_EVENT_DECLARE(is_connected_notification);

struct time_notification {
    uint8_t hour;
    uint8_t minute;
};

ZMK_EVENT_DECLARE(time_notification);

struct volume_notification {
    uint8_t value;
};

ZMK_EVENT_DECLARE(volume_notification);

struct layout_notification {
    uint8_t value;
};

ZMK_EVENT_DECLARE(layout_notification);

#endif