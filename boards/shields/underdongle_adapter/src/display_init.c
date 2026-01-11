#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/led.h>

static const struct device *pwm_leds_dev = DEVICE_DT_GET_ONE(pwm_leds);

#define DISP_BL DT_NODE_CHILD_IDX(DT_NODELABEL(disp_bl))

int display_init(void)
{
	// Set the orientation
	const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display))
	{
		return -EIO;
	}
	
	int ret = display_set_orientation(display, DISPLAY_ORIENTATION_ROTATED_270);
	if (ret < 0)
	{
		return ret;
	}

	// turn on backlight
	led_set_brightness(pwm_leds_dev, DISP_BL, 80);

	return 0;
}

SYS_INIT(display_init, APPLICATION, 60);