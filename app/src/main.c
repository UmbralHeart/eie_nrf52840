/*
 * main.c
 */

#include <zephyr/kernel.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

int main(void) {
  int pw_array[] = {0, 0, 0};
  int index = 0;

  if (0 > BTN_init()) {
    return 0;
  }

  if (0 > LED_init()) {
    return 0;
  }

  LED_toggle(LED0);

  while(1) {
    if (BTN_check_clear_pressed(BTN0)) {
      if (index == 0) 
        pw_array[0] = 1;
      index++;
    }
    else if (BTN_check_clear_pressed(BTN1))
    {
      if (index == 1) 
        pw_array[1] = 1;
      index++;
    }
    else if (BTN_check_clear_pressed(BTN2))
    {
      if (index == 2) 
        pw_array[2] = 1;
      index++;
    }
    else if (BTN_check_clear_pressed(BTN3))
    {
      if (pw_array[0] == 1 && pw_array[1] == 1 && pw_array[2] == 1)
        printk("Correct!\n");
      else
        printk("Incorrect!\n");

      LED_toggle(LED0);
      index = 0;
      pw_array[0] = 0;
      pw_array[1] = 0;
      pw_array[2] = 0;

      while (!(BTN_check_clear_pressed(BTN0) ||
             BTN_check_clear_pressed(BTN1) ||
             BTN_check_clear_pressed(BTN2) ||
             BTN_check_clear_pressed(BTN3))) {
        k_msleep(SLEEP_MS);
      }

      LED_toggle(LED0);
    }
    else;
  
  if (index > 3)
    pw_array[0] = 0;

  }
}
