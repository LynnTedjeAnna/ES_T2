#include <Arduino.h>

//-------------------------------------------------assignment_a------------------------------------------------------
#define BUILTIN_LED_MODE DDRB                   /* ddr (Data Direction Register) register for builtin led */
#define BUILTIN_LED_OUT PORTB                   /* port (output data) register for builtin led */
#define BUILTIN_LED_POS 7                       /* bit position for pin B7 (13) */
#define BUILTIN_LED_MASK (1 << BUILTIN_LED_POS) /* bitmask for pin B7 (13) */

#define BTN_MODE DDRG               /* ddr (Data Direction Register) register for button */
#define BTN_IN PING                 /* pin (input data) register for button */
#define BTN_POS 5                   /* bit position for pin G5 (4) */
#define BTN_MASK (1 << BTN_POS)     /* bitmask for pin G5 (4) */

#define GREEN_LED_MODE DDRB               /* ddr (Data Direction Register) register for green led */
#define GREEN_LED_OUT PORTB               /* port (output data) register for green led */
#define GREEN_LED_POS 6                   /* bitmask for pin B6 (12) */
#define GREEN_LED_MASK (1 << GREEN_LED_POS) /* bitmask for pin B6 (12) */

//-------------------------------------------------assignment_b------------------------------------------------------
#define BTN2_MODE DDRE                  /* ddr (Data Direction Register) register for button 2 */
#define BTN2_OUT PORTE                  /* port (output data) register for button 2 */
#define BTN2_IN PINE                    /* pin (input data) register for button 2*/
#define BTN2_POS 3                      /* bit position for pin E3 (5) */
#define BTN2_MASK (1 << BTN2_POS)       /* bitmask for pin E3 (5) */

#define RED_LED_MODE DDRH                   /* ddr (Data Direction Register) register for red led */
#define RED_LED_POS 6                       /* bitmask for pin H6 (9) */
#define RED_LED_MASK (1 << RED_LED_POS)     /* bitmask for pin H6 (9) */

#define YELLOW_LED_MODE DDRH                /* ddr (Data Direction Register) register for yellow led */
#define YELLOW_LED_POS 5                    /* bitmask for pin H5 (8) */
#define YELLOW_LED_MASK (1 << YELLOW_LED_POS) /* bitmask for pin H5 (8) */

#define BLUE_LED_MODE DDRH                 /* ddr (Data Direction Register) register for white led */
#define BLUE_LED_POS 4                     /* bitmask for pin H4 (7) */
#define BLUE_LED_MASK (1 << BLUE_LED_POS) /* bitmask for pin H4 (7) */

#define GREEN2_LED_MODE DDRH                /* ddr (Data Direction Register) register for green led */
#define GREEN2_LED_POS 3                   /* bitmask for pin H3 (6) */
#define GREEN2_LED_MASK (1 << GREEN2_LED_POS) /* bitmask for pin H3 (6) */

uint8_t debounce_delay = 50;
uint32_t prev = 0;

uint32_t prev_btn = 0;
uint8_t btn_state = 0;
uint8_t prev_btn_state = 0;

uint32_t prev_btn2 = 0;
uint8_t btn2_state = 0;
uint8_t prev_btn2_state = 0;

uint8_t sweep_reset_mask = 0b1111 << 3;
uint8_t sweep1_mask = 0b0001 << 3;
uint16_t sweep2_mask = 0b1111;

uint8_t count = 0;
int8_t count_state = 1;
uint8_t count2 = 0;
int8_t count_state2 = 1;

uint32_t sweep_delay = 400;
uint32_t sweep2_delay = 400;

uint32_t prev_sweep = 0;
uint32_t prev_sweep2 = 0;


void setup() {
    Serial.begin(9600);
    BUILTIN_LED_MODE |= BUILTIN_LED_MASK;    // set pin B7 as output, bit on is output, off is input
    GREEN_LED_MODE |= GREEN_LED_MASK;

    BTN_MODE &= ~BTN_MASK;                   // set pin D5 (Button) as input

    BTN2_OUT |= BTN2_MASK;                   // Enable internal pull-up resistor for Button 2
    BTN2_MODE &= ~BTN2_MASK;                 // Set pin D4 (Button 2) as input

    RED_LED_MODE |= RED_LED_MASK;
    YELLOW_LED_MODE |= YELLOW_LED_MASK;
    BLUE_LED_MODE |= BLUE_LED_MASK;
    GREEN2_LED_MODE |= GREEN2_LED_MASK;
}

void loop() {
    uint32_t now = millis();

//-------------------------------------------------assignment_a------------------------------------------------------
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

//-------------------------------------------------assignment_b------------------------------------------------------
    uint8_t btn2 = (BTN2_IN & BTN2_MASK) >> BTN2_POS;    // Read the current state of Button 2
    if (btn2 != prev_btn2_state) {              // Check if the state of Button 2 has changed
        prev_btn2 = now;                        // Reset the debouncing timer if state has changed
    }

    // Check if enough time has passed for debouncing
    if (now - prev_btn2 > debounce_delay) {
        if (btn2 != btn2_state) {
            btn2_state = btn2; // Update Button 2's state
        }
        // If Button 2 is pressed
        if (btn2_state == 0) {
            count = 0;
            if (now - prev_sweep2 > sweep2_delay) {
                prev_sweep2 = now;                  // Update the previous sweep time
                PORTH &= ~sweep_reset_mask;         // Reset the sweep output
                PORTH |= (sweep2_mask << count2);   // Set the new sweep output for Button 2
                // Update the count direction based on the current count2 value
                if (count2 == 6) {
                    count_state2 = -1;              // Change direction to down if count2 is 6
                } else if (count2 == 0) {
                    count_state2 = 1;               // Change direction to up if count2 is 0
                }
                count2 += count_state2;
            }
        // If Button 2 is not pressed
        } else if (now - prev_sweep > sweep_delay) {
            count2 = 0;
            prev_sweep = now;                   // Update the previous sweep time
            PORTH &= ~sweep_reset_mask;         // Reset the sweep output
            PORTH |= (sweep1_mask << count);    // Set the new sweep output for Button 2

            // Update the count direction based on the current count value
            if (count == 3) {
                count_state = -1;               // Change direction to down if count is 3
            } else if (count == 0) {
                count_state = 1;                // Change direction to up if count is 0
            }
            count += count_state;
        }
    }
    prev_btn2_state = btn2;  // Update previous Button 2 state
}