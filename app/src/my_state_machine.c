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
static void s0_state_entry(void* o);
static enum smf_state_result s0_state_run(void* o);
static void s0_state_exit(void* o);

static void s1_state_entry(void* o);
static enum smf_state_result s1_state_run(void* o);
static void s1_state_exit(void* o);

static void s2_state_entry(void* o);
static enum smf_state_result s2_state_run(void* o);
static void s2_state_exit(void* o);

static void s3_state_entry(void* o);
static enum smf_state_result s3_state_run(void* o);
static void s3_state_exit(void* o);

static void s4_state_entry(void* o);
static enum smf_state_result s4_state_run(void* o);
static void s4_state_exit(void* o);
/*-------------------------------------------------------------------
 * Typedefs
 *-------------------------------------------------------------------*/
enum led_state_machine_states {
    S0,
    S1,
    S2,
    S3,
    S4
};

typedef struct {
    // Context variable used by zephyr to track state machine state. Must be first
    struct smf_ctx ctx;

    uint16_t count;
} led_state_object_t;

/*-------------------------------------------------------------------
 * Local Variables
 *-------------------------------------------------------------------*/
static const struct smf_state led_states[] = {
    [S0] = SMF_CREATE_STATE(s0_state_entry, s0_state_run, s0_state_exit, NULL, NULL),
    [S1] = SMF_CREATE_STATE(s1_state_entry, s1_state_run, s1_state_exit, NULL, NULL),
    [S2] = SMF_CREATE_STATE(s2_state_entry, s2_state_run, s2_state_exit, NULL, NULL),
    [S3] = SMF_CREATE_STATE(s3_state_entry, s3_state_run, s3_state_exit, NULL, NULL),
    [S4] = SMF_CREATE_STATE(s4_state_entry, s4_state_run, s4_state_exit, NULL, NULL),
};

static led_state_object_t led_state_object;

void state_machine_init() {
    led_state_object.count = 0;
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[S0]);
}

int state_machine_run() {
    return smf_run_state(SMF_CTX(&led_state_object));
}

static void s0_state_entry(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    printk("Entering S0\n");
}

static enum smf_state_result s0_state_run(void* o) {
    if (BTN_check_clear_pressed(BTN0)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S1]);
    }
        
    return SMF_EVENT_HANDLED;
}

  static void s0_state_exit(void* o) {
    printk("Exiting S0\n");
}

  static void s1_state_entry(void* o) {
    LED_set(LED0, LED_ON);
    printk("Entering S1\n");
}

static enum smf_state_result s1_state_run(void* o) {
    if (BTN_check_clear_pressed(BTN1)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S2]);
    }
    else if (BTN_check_clear_pressed(BTN2)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S4]);
    }
    else if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    else if (led_state_object.count > 250) {
        led_state_object.count = 0;
        LED_toggle(LED0);
    }
    else
        led_state_object.count++;

    return SMF_EVENT_HANDLED;
}

static void s1_state_exit(void* o) {
    printk("Exiting S1\n");
}

static void s2_state_entry(void* o) {
    LED_set(LED0, LED_ON);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_ON);
    LED_set(LED3, LED_OFF);
    printk("Entering S2\n");
}

static enum smf_state_result s2_state_run(void* o) {
    if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    else if (led_state_object.count > 1000) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S3]);
    }
    else
        led_state_object.count++;

    return SMF_EVENT_HANDLED;
}

static void s2_state_exit(void* o) {
    printk("Exiting S2\n");
}

static void s3_state_entry(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_ON);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_ON);
    printk("Entering S3\n");
}

static enum smf_state_result s3_state_run(void* o) {
    if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    else if (led_state_object.count > 2000) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S2]);
    }
    else
        led_state_object.count++;

    return SMF_EVENT_HANDLED;
}

static void s3_state_exit(void* o) {
    printk("Exiting S3\n");
}

static void s4_state_entry(void* o) {
    LED_set(LED0, LED_ON);
    LED_set(LED1, LED_ON);
    LED_set(LED2, LED_ON);
    LED_set(LED3, LED_ON);
    printk("Entering S4\n");
}

static enum smf_state_result s4_state_run(void* o) {
    if (BTN_check_clear_pressed(BTN3)) {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    else if (led_state_object.count > 63) {
          LED_toggle(LED0);
          LED_toggle(LED1);
          LED_toggle(LED2);
          LED_toggle(LED3);
          led_state_object.count = 0;
    }
    else
        led_state_object.count++;

    return SMF_EVENT_HANDLED;
}

static void s4_state_exit(void* o) {
    printk("Exiting S4\n");
}