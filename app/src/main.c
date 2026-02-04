/**
 * @file main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <lvgl.h>

#include "BTN.h"
#include "LED.h"
#include "lv_data_obj.h"

#define SLEEP_MS 100

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static lv_obj_t *screen = NULL;

void lv_slider_event_cb(lv_event_t *event) {
  lv_obj_t *slider = lv_event_get_target(event);

  int32_t slider_value = lv_slider_get_value(slider);
  
  LED_pwm(LED0, (uint8_t)slider_value);

  
}

int main(void) {
  if (!device_is_ready(display_dev)) {
    return 0;
  }

  screen = lv_screen_active();
  if (screen == NULL) {
    return 0;
  }

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }
  

  lv_obj_t *slider = lv_slider_create(screen);
  lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *slider_label = lv_label_create(slider);
  char label_buf[16];
  snprintf(label_buf, 16, "LED slider");
  lv_label_set_text(slider_label, label_buf);
  lv_obj_align(slider_label, LV_ALIGN_CENTER, 0, 0);

  lv_obj_add_event_cb(slider, lv_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  display_blanking_off(display_dev);
  while (1) {
    lv_timer_handler();
    k_msleep(SLEEP_MS);
  }
  return 0;
}
