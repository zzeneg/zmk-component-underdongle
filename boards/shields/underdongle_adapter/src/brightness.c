#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/led.h>

static const struct device *pwm_leds_dev = DEVICE_DT_GET_ONE(pwm_leds);

#define DISP_BL DT_NODE_CHILD_IDX(DT_NODELABEL(disp_bl))

static int init_fixed_brightness(void) {
    led_set_brightness(pwm_leds_dev, DISP_BL, 50);

    return 0;
}

SYS_INIT(init_fixed_brightness, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
