/*
 * main.c
 */

#include <zephyr/kernel.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

int main(void) {
  int pw_array[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int pw_entry[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int index = 0;
  int timer = 0;
  int init_index = 0;
  int flag0 = 0;
  int flag = 0;

  if (0 > BTN_init()) {
    return 0;
  }

  if (0 > LED_init()) {
    return 0;
  }

  LED_toggle(LED3);
  while (timer < 3000) {
    if (BTN_check_clear_pressed(BTN3)) {

      while (1) {  
        if(BTN_check_clear_pressed(BTN0)) {
          pw_array[init_index] = 1;
          init_index++;
        }

        else if (BTN_check_clear_pressed(BTN1))
        {
          pw_array[init_index] = 2;
          init_index++;
        }

        else if (BTN_check_clear_pressed(BTN2))
        {
          pw_array[init_index] = 3;
          init_index++;
        }

        else if (BTN_check_clear_pressed(BTN3))
        {
          LED_toggle(LED3);
          while (!(BTN_check_clear_pressed(BTN0) ||
                  BTN_check_clear_pressed(BTN1) ||
                  BTN_check_clear_pressed(BTN2) ||
                  BTN_check_clear_pressed(BTN3))) {
            k_msleep(SLEEP_MS);
          }
        flag0 = 1;
        break;
        }

        else;

        if (init_index > 10){
          flag = -1;
          while (!BTN_check_clear_pressed(BTN3)) {
            k_msleep(SLEEP_MS);
          }
        }

      }

    }

    if (flag0 == 1)
      break;

    k_msleep(50);
    timer += 50;
  }
  printk("%d %d %d %d %d %d %d %d %d %d\n", pw_array[0], pw_array[1], pw_array[2], pw_array[3], pw_array[4], pw_array[5], pw_array[6], pw_array[7], pw_array[8], pw_array[9]);
  LED_set(LED3, 0);
  LED_toggle(LED0);

  while(1) {
    if (BTN_check_clear_pressed(BTN0)) {
      pw_entry[index] = 1;
      index++;
    }
    else if (BTN_check_clear_pressed(BTN1))
    {
      pw_entry[index] = 2;
      index++;
    }
    else if (BTN_check_clear_pressed(BTN2))
    {
      pw_entry[index] = 3;
      index++;
    }
    else if (BTN_check_clear_pressed(BTN3))
    {
      printk("%d %d %d %d %d %d %d %d %d %d\n", pw_entry[0], pw_entry[1], pw_entry[2], pw_entry[3], pw_entry[4], pw_entry[5], pw_entry[6], pw_entry[7], pw_entry[8], pw_entry[9]);
      if (pw_array[0] == pw_entry[0] &&
          pw_array[1] == pw_entry[1] &&
          pw_array[2] == pw_entry[2] &&
          pw_array[3] == pw_entry[3] &&
          pw_array[4] == pw_entry[4] &&
          pw_array[5] == pw_entry[5] &&
          pw_array[6] == pw_entry[6] &&
          pw_array[7] == pw_entry[7] &&
          pw_array[8] == pw_entry[8] &&
          pw_array[9] == pw_entry[9] &&
          flag != -1)
        printk("Correct!\n");
      else
        printk("Incorrect!\n");

      LED_toggle(LED0);

      index = 0;
      pw_entry[0] = 0;
      pw_entry[1] = 0;
      pw_entry[2] = 0;
      pw_entry[3] = 0;
      pw_entry[4] = 0;
      pw_entry[5] = 0;
      pw_entry[6] = 0;
      pw_entry[7] = 0;
      pw_entry[8] = 0;
      pw_entry[9] = 0;

      while (!(BTN_check_clear_pressed(BTN0) ||
             BTN_check_clear_pressed(BTN1) ||
             BTN_check_clear_pressed(BTN2) ||
             BTN_check_clear_pressed(BTN3))) {
        k_msleep(SLEEP_MS);
      }

      LED_toggle(LED0);
    }
    else;
  
  if (index > 10)
    flag = -1;

  }
}
