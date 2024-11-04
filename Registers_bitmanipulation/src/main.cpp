#include <Arduino.h>

#define BUILTIN_LED_MODE DDRB                   /* ddr (Data Direction Register) register for builtin led */
#define BUILTIN_LED_OUT PORTB                   /* port (output data) register for builtin led */
#define BUILTIN_LED_POS 7                       /* bit position for pin B7 (13) */
#define BUILTIN_LED_MASK (1 << BUILTIN_LED_POS) /* bitmask for pin B7 (13) */

#define BTN_MODE DDRG               /* ddr (Data Direction Register) register for button */
#define BTN_IN PING                 /* pin (input data) register for button */
#define BTN_POS 5                   /* bit position for pin G5 (4) */
#define BTN_MASK (1 << BTN_POS)     /* bitmask for pin G5 (4) */

#define GREEN_LED_MODE DDRB               /* ddr (Data Direction Register) register for red led */
#define GREEN_LED_OUT PORTB               /* port (output data) register for red led */
#define GREEN_LED_POS 6                   /* bitmask for pin B6 (12) */
#define GREEN_LED_MASK (1 << GREEN_LED_POS) /* bitmask for pin B6 (12) */

uint32_t prev = 0;
uint32_t prev_btn = 0;
uint8_t debounce_delay = 50;
uint8_t btn_state = 0;
uint8_t prev_btn_state = 0;


void setup() {
    BUILTIN_LED_MODE |= BUILTIN_LED_MASK;    // set pin B5 as output, bit on is output, off is input
    GREEN_LED_MODE |= GREEN_LED_MASK;
    BTN_MODE &= ~BTN_MASK;                   // set pin D7 as input
}

void loop() {
    uint32_t now = millis();

    if (now - prev > 125) {
        GREEN_LED_OUT ^= GREEN_LED_MASK;  // toggle green LED on
        prev = now;
    }

    uint8_t btn = (BTN_IN & BTN_MASK) >> BTN_POS;  // read button

    if (btn != prev_btn_state) {
        prev_btn = now;    // reset the debouncing timer
    }

    // Check if the button state has stabilised
    if (now - prev_btn > debounce_delay) {
        if (btn != btn_state) {
            btn_state = btn; // Update the button state to current state
        }
        if (btn_state == 0) {
            BUILTIN_LED_OUT |= BUILTIN_LED_MASK;   // turn build in led on
        } else {
            BUILTIN_LED_OUT &= ~BUILTIN_LED_MASK;   // turn build in led off
        }
    }

    // Update the previous button state
    prev_btn_state = btn;
}

