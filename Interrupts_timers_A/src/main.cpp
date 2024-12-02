#include <Arduino.h>

/*  ASSIGNMENT
 * • When both buttons D10 and D11 are pressed then both LEDs are blinking with
 * alternating pattern repeatedly every 0.5 seconds.
 * */

#define GREEN_BTN_MODE DDRB                     /* ddr (Data Direction Register) register for button */
#define GREEN_BTN_IN PINB                       /* pin (input data) register for button */
#define GREEN_BTN_POS 3                         /* bit position for pin B3 (11) */
#define GREEN_BTN_MASK (1 << GREEN_BTN_POS)     /* bitmask for pin B3 (11) */

#define YELLOW_BTN_MODE DDRB                    /* ddr (Data Direction Register) register for button */
#define YELLOW_BTN_IN PINB                       /* pin (input data) register for button */
#define YELLOW_BTN_POS 2                        /* bit position for pin B2 (10) */
#define YELLOW_BTN_MASK (1 << YELLOW_BTN_POS)   /* bitmask for pin B2 (10) */

#define GREEN_LED_MODE DDRD                     /* ddr (Data Direction Register) register for led */
#define GREEN_LED_OUT PORTD                     /* port (output data) register for green led */
#define GREEN_LED_POS 6                         /* bitmask for pin D6 (6) */
#define GREEN_LED_MASK (1 << GREEN_LED_POS)     /* bitmask for pin D6 (6) */

#define YELLOW_LED_MODE DDRD                    /* ddr (Data Direction Register) register for led */
#define YELLOW_LED_OUT PORTD                    /* port (output data) register for green led */
#define YELLOW_LED_POS 5                        /* bitmask for pin D5 (5) */
#define YELLOW_LED_MASK (1 << YELLOW_LED_POS)   /* bitmask for pin D5 (5) */

uint8_t debounce_delay = 50;
uint32_t prev = 0;

uint32_t prev_grn_btn = 0;
uint8_t grn_btn_state = 0;
uint8_t prev_grn_btn_state = 0;

uint32_t prev_ylw_btn = 0;
uint8_t ylw_btn_state = 0;
uint8_t prev_ylw_btn_state = 0;

void setup() {
    Serial.begin(9600);

    YELLOW_BTN_MODE &= ~YELLOW_BTN_MASK;    // set pin B4 as input
    GREEN_BTN_MODE &= ~GREEN_BTN_MASK;      // set pin B3 as input

    GREEN_LED_MODE |= GREEN_LED_MASK;       // set pin D6 as output
    YELLOW_LED_MODE |= YELLOW_LED_MASK;     // set pin D5 as output
}

void loop() {
    uint32_t now = millis();

    uint8_t ylw_btn = (YELLOW_BTN_IN & YELLOW_BTN_MASK) >> YELLOW_BTN_POS;  // read button
    uint8_t grn_btn = (GREEN_BTN_IN & GREEN_BTN_MASK) >> GREEN_BTN_POS;     // read button

    if (ylw_btn != prev_ylw_btn_state) {
        prev_ylw_btn = now;    // reset the debouncing timer
    }
    if (grn_btn != prev_grn_btn_state) {
        prev_grn_btn = now;    // reset the debouncing timer
    }

    // Check if the button state has stabilised
    if (now - prev_ylw_btn > debounce_delay) {
        if (ylw_btn != ylw_btn_state) { ylw_btn_state = ylw_btn; }  // Update the button state to current state

        if (ylw_btn == 0) {
            YELLOW_LED_OUT |= YELLOW_LED_MASK;   // turn yellow led on
        } else {
            YELLOW_LED_OUT &= ~YELLOW_LED_MASK;   // turn yellow led off
        }
    }

    // Check if the button state has stabilised
    if (now - prev_ylw_btn > debounce_delay) {
        if (grn_btn != grn_btn_state) { grn_btn_state = grn_btn; }  // Update the button state to current state

        if (grn_btn == 0) {
            GREEN_LED_OUT |= GREEN_LED_MASK;   // turn green led on
            Serial.print("Hello World!\n");
        } else {
            GREEN_LED_OUT &= ~GREEN_LED_MASK;   // turn green led off
        }
    }

    // Check if the button states has stabilised (Checking one is fine, but want to be visible that I am using both buttons)
    if (now - prev_ylw_btn > debounce_delay && now - prev_ylw_btn > debounce_delay ) {
        if (grn_btn != grn_btn_state) { grn_btn_state = grn_btn; }  // Update the button state to current state
        if (ylw_btn != ylw_btn_state) { ylw_btn_state = ylw_btn; }  // Update the button state to current state

        if (grn_btn == 0 && ylw_btn == 0) {
            // logic

        } else {
            GREEN_LED_OUT &= ~GREEN_LED_MASK;       // turn green led off
            YELLOW_LED_OUT &= ~YELLOW_LED_MASK;     // turn yellow led off
        }
    }

    // Update the previous button states
    prev_ylw_btn_state = ylw_btn;
    prev_grn_btn_state = grn_btn;

    delay(200);
}