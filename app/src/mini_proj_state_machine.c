/**
 * @file mini_proj_state_machine.c
 */

 #include <zephyr/smf.h>
 #include "LED.h"
 #include "BTN.h"
 #include "mini_proj_state_machine.h"

/*-------------------------------------------------------------------
 * Function Prototypes
 *-------------------------------------------------------------------*/

static int8_t BTN_0_1_held_for_3s(void);
void reset_current_char_array(void);

static void enter_ascii_entry(void* o);
static enum smf_state_result enter_ascii_run(void* o);

static void saved_char_entry(void* o);
static enum smf_state_result saved_char_run(void* o);

static void saved_string_entry(void* o);
static enum smf_state_result saved_string_run(void* o);

static void standby_entry(void* o);
static enum smf_state_result standby_run(void* o);

/*-------------------------------------------------------------------
 * Typedefs
 *-------------------------------------------------------------------*/


enum mini_proj_machine_states {
    enter_ascii,
    saved_char,
    saved_string,
    standby,
};

typedef struct {
    // Context variable used by zephyr to track state machine state. Must be first
    struct smf_ctx ctx;

    uint16_t counter;

    uint8_t ascii_index;
    uint8_t current_char[7];

    uint8_t saved_char_index;
    char saved_string[10];

    uint8_t state_before_standby;

} mini_proj_state_object_t;


/*-------------------------------------------------------------------
 * Local Variables
 *-------------------------------------------------------------------*/

static const struct smf_state mini_proj_states[] = {
    [enter_ascii] = SMF_CREATE_STATE(enter_ascii_entry, enter_ascii_run, NULL, NULL, NULL),
    [saved_char] = SMF_CREATE_STATE(saved_char_entry, saved_char_run, NULL, NULL, NULL),
    [saved_string] = SMF_CREATE_STATE(saved_string_entry, saved_string_run, NULL, NULL, NULL),
    [standby] = SMF_CREATE_STATE(standby_entry, standby_run, NULL, NULL, NULL),
};
static mini_proj_state_object_t mini_proj_state_object;

void reset_current_char_array(void) {
    for (int i = 0; i < 7; i++) 
        mini_proj_state_object.current_char[i] = 0;
}

void state_machine_init() {
    smf_set_initial(SMF_CTX(&mini_proj_state_object), &mini_proj_states[enter_ascii]);
    mini_proj_state_object.counter = 0;
    mini_proj_state_object.ascii_index = 6;
    reset_current_char_array();
 }

void state_machine_run() {
    smf_run_state(SMF_CTX(&mini_proj_state_object));
}

static int8_t BTN_0_1_held_for_3s(void) {
    static uint16_t btn0_press_time = 0;
    static uint16_t btn1_press_time = 0;

    if (BTN_check_clear_pressed(BTN0) && BTN_check_clear_pressed(BTN1)) {
        btn0_press_time++;
        btn1_press_time++;
        
        if (btn0_press_time > 3000 && btn1_press_time > 3000) {
            btn0_press_time = 0;
            btn1_press_time = 0;
            return 1;
        }
    } else {
        btn0_press_time = 0;
        btn1_press_time = 0;
    }

    return 0;
}

static void enter_ascii_entry(void* o) {
    LED_set(LED3, LED_ON);
    mini_proj_state_object.ascii_index = 6;
    reset_current_char_array();
}

static enum smf_state_result enter_ascii_run(void* o) {

    if (BTN_0_1_held_for_3s()) {
        mini_proj_state_object.state_before_standby = 1;
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[standby]);
    }
    
    else if (BTN_check_clear_pressed(BTN0) && !BTN_check_clear_pressed(BTN1) && mini_proj_state_object.ascii_index >= 0) {
        mini_proj_state_object.current_char[mini_proj_state_object.ascii_index] = 0;
        mini_proj_state_object.ascii_index--;
    }
    
    else if (BTN_check_clear_pressed(BTN1) && !BTN_check_clear_pressed(BTN0) && mini_proj_state_object.ascii_index >= 0) {
        mini_proj_state_object.current_char[mini_proj_state_object.ascii_index] = 1;
        mini_proj_state_object.ascii_index--;
    }
    
    else if (BTN_check_clear_pressed(BTN2))  {
        reset_current_char_array();
        mini_proj_state_object.ascii_index = 6;
    }

    else if (BTN_check_clear_pressed(BTN3)) 
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[saved_char]);

    if (mini_proj_state_object.counter > 1000) {
        LED_toggle(LED3);
        mini_proj_state_object.counter = 0;
    } else {
        mini_proj_state_object.counter++;
    }
    
    return SMF_EVENT_HANDLED;

}

static void saved_char_entry(void* o) {

    if (mini_proj_state_object.saved_char_index >= 10) {
        mini_proj_state_object.saved_char_index = 9; // Prevent overflow
    }

    for (int i = 0; i < 7; i++) {
        if (mini_proj_state_object.current_char[i] == 1) {
            mini_proj_state_object.saved_string[mini_proj_state_object.saved_char_index] += 2**i;
        }
    }
    prinktk("Saved Char: %c\n", mini_proj_state_object.saved_string[mini_proj_state_object.saved_char_index]);

    mini_proj_state_object.saved_char_index++;
}

static enum smf_state_result saved_char_run(void* o) {
    
    if (BTN_0_1_held_for_3s()) {
        mini_proj_state_object.state_before_standby = 2;
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[standby]);
    }

    else if (BTN_check_clear_pressed(BTN0) ^ BTN_check_clear_pressed(BTN1)) {
        // Move to the next character
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[enter_ascii]);
    }

    else if (BTN_check_clear_pressed(BTN2)) {
        // Delete string and go back to enter ascii state
        mini_proj_state_object.saved_char_index = 0;
        mini_proj_state_object.saved_string[0] = '\0';
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[enter_ascii]);
    }

    else if (BTN_check_clear_pressed(BTN3)) {
        // Move to saved string state
        mini_proj_state_object.saved_string[mini_proj_state_object.saved_char_index] = '\0';
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[saved_string]);
    }

    if (mini_proj_state_object.counter > 250) {
        LED_toggle(LED3);
        mini_proj_state_object.counter = 0;
    } else {
        mini_proj_state_object.counter++;
    }

    return SMF_EVENT_HANDLED;
    
}

static void saved_string_entry(void* o) {
    // No special entry actions needed
}

static enum smf_state_result saved_string_run(void* o) {
    
    if (BTN_0_1_held_for_3s()) {
        mini_proj_state_object.state_before_standby = 3;
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[standby]);
    }

    else if (BTN_check_clear_pressed(BTN2)) {
        // Go back to enter ascii state
        mini_proj_state_object.saved_char_index = 0;
        mini_proj_state_object.saved_string[0] = '\0';
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[enter_ascii]);
    }
    else if (BTN_check_clear_pressed(BTN3)) {
        // Send to the serial monitor
        printk("Saved String: %s\n", mini_proj_state_object.saved_string);
    }

    if (mini_proj_state_object.counter > 62.5) {
        LED_toggle(LED3);
        mini_proj_state_object.counter = 0;
    } else {
        mini_proj_state_object.counter++;
    }

    return SMF_EVENT_HANDLED;
}

static void standby_entry(void* o) {
    LED_set(LED3, LED_OFF);
}

static enum smf_state_result standby_run(void* o) {
    uint8_t current_duty_cycle = 0;
    while (!BTN_check_clear_pressed(BTN0) 
            || !BTN_check_clear_pressed(BTN1) 
            || !BTN_check_clear_pressed(BTN2) 
            || !BTN_check_clear_pressed(BTN3)) {

        LED_pwm(LED0, current_duty_cycle);
        LED_pwm(LED1, current_duty_cycle);
        current_duty_cycle += 5;
        if (current_duty_cycle > 100) {
            current_duty_cycle = 0;
        }
        k_msleep(100);
    }
    // Restore previous state
    if (mini_proj_state_object.state_before_standby == 1)
    {
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[enter_ascii]);
    }
    else if (mini_proj_state_object.state_before_standby == 2)
    {
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[saved_char]);
    }
    else // if (mini_proj_state_object.state_before_standby == 3)
    {
        smf_set_state(SMF_CTX(&mini_proj_states), &mini_proj_states[saved_string]);
    }
    return SMF_EVENT_HANDLED;
}

