/**
 * @file my_state_machine.c
 */

#include <zephyr/smf.h>
#include "BTN.h" 
#include "LED.h"
#include "my_state_machine.h"

/*-------------------------------------------------------------------
 * Function Prototypes
 *-------------------------------------------------------------------*/
static void startup_entry(void* o);
static enum smf_state_result startup_run(void* o);

static enum smf_state_result enter_pw_run(void* o);

static void password_entry(void* o);
static enum smf_state_result password_run(void* o);

static void locked_entry(void* o);
static enum smf_state_result locked_run(void* o);

/*-------------------------------------------------------------------
 * Typedefs
 *-------------------------------------------------------------------*/
enum led_state_machine_states {
    startup,
    enter_pw,
    password,
    locked
};

typedef struct {
    // Context variable used by zephyr to track state machine state. Must be first
    struct smf_ctx ctx;

    uint16_t count;

    uint8_t pw_array[4];

    uint8_t try_pw_array[4];

    uint8_t flag;

    uint8_t index;
} led_state_object_t;

/*-------------------------------------------------------------------
 * Local Variables
 *-------------------------------------------------------------------*/
static const struct smf_state led_states[] = {
    [startup] = SMF_CREATE_STATE(startup_entry, startup_run, NULL, NULL, NULL),
    [enter_pw] = SMF_CREATE_STATE(NULL, enter_pw_run, NULL, NULL, NULL),
    [password] = SMF_CREATE_STATE(password_entry, password_run, NULL, NULL, NULL),
    [locked] = SMF_CREATE_STATE(locked_entry, locked_run, NULL, NULL, NULL),
};

static led_state_object_t led_state_object;

void state_machine_init() {
    led_state_object.count = 0;
    led_state_object.flag = 0;
    led_state_object.pw_array[0] = 0;
    led_state_object.pw_array[1] = 0;
    led_state_object.pw_array[2] = 0;
    led_state_object.pw_array[3] = 0;
    led_state_object.try_pw_array[0] = 0;
    led_state_object.try_pw_array[1] = 0;
    led_state_object.try_pw_array[2] = 0;
    led_state_object.try_pw_array[3] = 0;
    led_state_object.index = 0;
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[startup]);
}

int state_machine_run() {
    return smf_run_state(SMF_CTX(&led_state_object));
}

static void startup_entry(void* o) {
    LED_set(LED3, LED_ON);
}

static enum smf_state_result startup_run(void* o) {
    if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[enter_pw]);
    }
    else if (led_state_object.count > 3000) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[password]);
    }
    else
        led_state_object.count++;
        
    return SMF_EVENT_HANDLED;
}

static enum smf_state_result enter_pw_run(void* o) {
    if (BTN_check_clear_pressed(BTN0) && led_state_object.flag != 1) {
        led_state_object.pw_array[led_state_object.index] = 1;
        led_state_object.index++;
    }
    else if (BTN_check_clear_pressed(BTN1) && led_state_object.flag != 1) {
        led_state_object.pw_array[led_state_object.index] = 2;
        led_state_object.index++;
    }
    else if (BTN_check_clear_pressed(BTN2) && led_state_object.flag != 1) {
        led_state_object.pw_array[led_state_object.index] = 3;
        led_state_object.index++;
    }
    else if (BTN_check_clear_pressed(BTN3)) {
        printk("%d %d %d %d\n", led_state_object.pw_array[0], led_state_object.pw_array[1], led_state_object.pw_array[2], led_state_object.pw_array[3]);
        smf_set_state(SMF_CTX(&led_state_object), &led_states[locked]);
    }

    if (led_state_object.index >= 4) {
        led_state_object.flag = 1;
    }

    return SMF_EVENT_HANDLED;
}

static void locked_entry(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
}

static enum smf_state_result locked_run(void* o) {
    if (BTN_check_clear_pressed(BTN0) ||
        BTN_check_clear_pressed(BTN1) ||
        BTN_check_clear_pressed(BTN2) ||
        BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[password]);
    }

    return SMF_EVENT_HANDLED;
}
static void password_entry(void* o) {
    LED_set(LED0, LED_ON);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    led_state_object.index = 0;
}

static enum smf_state_result password_run(void* o) {
    if (BTN_check_clear_pressed(BTN0) && led_state_object.flag != 1) {
        led_state_object.try_pw_array[led_state_object.index] = 1;
        led_state_object.index++;
    }
    else if (BTN_check_clear_pressed(BTN1) && led_state_object.flag != 1) {
        led_state_object.try_pw_array[led_state_object.index] = 2;
        led_state_object.index++;
    }
    else if (BTN_check_clear_pressed(BTN2) && led_state_object.flag != 1) {
        led_state_object.try_pw_array[led_state_object.index] = 3;
        led_state_object.index++;
    }
    else if (BTN_check_clear_pressed(BTN3)) {
        printk("%d %d %d %d\n", led_state_object.try_pw_array[0], led_state_object.try_pw_array[1], led_state_object.try_pw_array[2], led_state_object.try_pw_array[3]);
        if (led_state_object.try_pw_array[0] == led_state_object.pw_array[0] &&
            led_state_object.try_pw_array[1] == led_state_object.pw_array[1] &&
            led_state_object.try_pw_array[2] == led_state_object.pw_array[2] &&
            led_state_object.try_pw_array[3] == led_state_object.pw_array[3] &&
            led_state_object.flag != 1) {
            printk("Correct!\n");
        }
        else {
            printk("Incorrect!\n");
        }
        smf_set_state(SMF_CTX(&led_state_object), &led_states[locked]);
    }

    if (led_state_object.index >= 4) {
        led_state_object.flag = 1;
    }

    return SMF_EVENT_HANDLED;
}