/*
 * main.c
 */

#include <zephyr/kernel.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

int main(void) {
  int counter = 0;

  if (0 > BTN_init()) {
    return 0;
  }

  if (0 > LED_init()) {
    return 0;
  }


  while(1) {
    
    if (BTN_check_clear_pressed(BTN0))
      counter++;
    
    switch (counter) {
      case 1:
        LED_set(LED0, 1);
        LED_set(LED1, 0);
        LED_set(LED2, 0);
        LED_set(LED3, 0);
        break;
      case 2:
        LED_set(LED0, 0);
        LED_set(LED1, 1);
        LED_set(LED2, 0);
        LED_set(LED3, 0);
        break;
      case 3:
        LED_set(LED0, 1);
        LED_set(LED1, 1);
        LED_set(LED2, 0);
        LED_set(LED3, 0);
        break;
      case 4:
        LED_set(LED0, 0);
        LED_set(LED1, 0);
        LED_set(LED2, 1);
        LED_set(LED3, 0);
        break;
      case 5:
        LED_set(LED0, 1);
        LED_set(LED1, 0);
        LED_set(LED2, 1);
        LED_set(LED3, 0);
        break;
      case 6:
        LED_set(LED0, 0);
        LED_set(LED1, 1);
        LED_set(LED2, 1);
        LED_set(LED3, 0);
        break;
      case 7:
        LED_set(LED0, 1);
        LED_set(LED1, 1);
        LED_set(LED2, 1);
        LED_set(LED3, 0);
        break;
      case 8:
        LED_set(LED0, 0);
        LED_set(LED1, 0);
        LED_set(LED2, 0);
        LED_set(LED3, 1);
        break;
      case 9:
        LED_set(LED0, 1);
        LED_set(LED1, 0);
        LED_set(LED2, 0);
        LED_set(LED3, 1);
        break;
      case 10:
        LED_set(LED0, 0);
        LED_set(LED1, 1);
        LED_set(LED2, 0);
        LED_set(LED3, 1);
        break;
      case 11:
        LED_set(LED0, 1);
        LED_set(LED1, 1);
        LED_set(LED2, 0);
        LED_set(LED3, 1);
        break;
      case 12:
        LED_set(LED0, 0);
        LED_set(LED1, 0);
        LED_set(LED2, 1);
        LED_set(LED3, 1);
        break;
      case 13:
        LED_set(LED0, 1);
        LED_set(LED1, 0);
        LED_set(LED2, 1);
        LED_set(LED3, 1);
        break;
      case 14:
        LED_set(LED0, 0);
        LED_set(LED1, 1);
        LED_set(LED2, 1);
        LED_set(LED3, 1);
        break;
      case 15:
        LED_set(LED0, 1);
        LED_set(LED1, 1);
        LED_set(LED2, 1);
        LED_set(LED3, 1);
        break;
      case 16:
        LED_set(LED0, 0);
        LED_set(LED1, 0);
        LED_set(LED2, 0);
        LED_set(LED3, 0);
        counter = 0;
        break;
      default:
        break;
    }
  }
}
