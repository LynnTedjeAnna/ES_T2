#include <Arduino.h>

#define GREEN_LED_MODE DDRD                     /* ddr (Data Direction Register) register for led */
#define GREEN_LED_OUT PORTD                     /* port (output data) register for green led */
#define GREEN_LED_POS 6                         /* bitmask for pin D6 (6) */
#define GREEN_LED_MASK (1 << GREEN_LED_POS)     /* bitmask for pin D6 (6) */

#define YELLOW_LED_MODE DDRD                    /* ddr (Data Direction Register) register for led */
#define YELLOW_LED_OUT PORTD                    /* port (output data) register for green led */
#define YELLOW_LED_POS 5                        /* bitmask for pin D5 (5) */
#define YELLOW_LED_MASK (1 << YELLOW_LED_POS)   /* bitmask for pin D5 (5) */

const uint8_t debounce_delay = 50;
const uint16_t sequence_delay = 250;
uint32_t prev_sequence = 0;
uint8_t led = 0;

volatile uint32_t prev_grn_btn = 0;
volatile uint8_t grn_btn_state = 0;
volatile uint8_t prev_grn_btn_state = 0;

volatile uint32_t prev_ylw_btn = 0;
volatile uint8_t ylw_btn_state = 0;
volatile uint8_t prev_ylw_btn_state = 0;

volatile uint8_t grn_led_state = 0;           // State of the green LED
volatile uint8_t ylw_led_state = 0;          // State of the yellow LED

void setup() {
    Serial.begin(9600);
    GREEN_LED_MODE |= GREEN_LED_MASK;       // set pin D6 as output
    YELLOW_LED_MODE |= YELLOW_LED_MASK;     // set pin D5 as output

    GREEN_LED_OUT &= ~GREEN_LED_MASK;       // Initialize led to off
    YELLOW_LED_OUT &= ~YELLOW_LED_MASK;     // Initialize led to off

    // Configure INT0 for the Green Button on toggle
    EICRA |= (1 << ISC00);                  // Set ISC00 to 1
    EICRA &= ~(1 << ISC01);                 // Clear ISC01 to 1
    EIMSK |= (1 << INT0);                   // Enable INT0 interrupt

    // Configure INT1 for the Yellow Button on toggle
    EICRA |= (1 << ISC10);                  // Set ISC10 to 1
    EICRA &= ~(1 << ISC11);                 // Clear ISC11 to 1
    EIMSK |= (1 << INT1);                   // Enable INT1 interrupt

    sei();  // Enable global interrupts
}

ISR(INT0_vect) {
    uint32_t now = millis();
    if (now - prev_ylw_btn > debounce_delay) {  // debounce logic
        YELLOW_LED_OUT ^= YELLOW_LED_MASK;      // toggle yellow led on
        prev_ylw_btn = now;                     // Update timestamp
        ylw_btn_state = !ylw_btn_state;
    }
}

ISR(INT1_vect) {
    uint32_t now = millis();
    if (now - prev_grn_btn > debounce_delay) {  // debounce logic
        GREEN_LED_OUT ^= GREEN_LED_MASK;        // toggle green led on
        prev_grn_btn = now;                     // Update timestamp
        grn_btn_state = !grn_btn_state;
    }
}

void loop() {
    uint32_t now = millis();

    // If both buttons are pressed, alternate LEDs every 0.5 seconds
    if (grn_btn_state && ylw_btn_state) {
        if (now - prev_sequence > sequence_delay) {
            prev_sequence = now;  // Update the previous sequence time

            // Alternate LEDs
            if (led) {
                GREEN_LED_OUT |= GREEN_LED_MASK;    // turn green LED on
                YELLOW_LED_OUT &= ~YELLOW_LED_MASK; // turn yellow LED off
                led = 0;
            } else {
                GREEN_LED_OUT &= ~GREEN_LED_MASK;   // turn green LED off
                YELLOW_LED_OUT |= YELLOW_LED_MASK;  // turn yellow LED on
                led = 1;
            }
        }
    }
    if (!ylw_btn_state){ YELLOW_LED_OUT &= ~YELLOW_LED_MASK;}
    if (!grn_btn_state){GREEN_LED_OUT &= ~GREEN_LED_MASK;}
}