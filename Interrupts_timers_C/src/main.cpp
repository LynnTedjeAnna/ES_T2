/*
 * • Timer 1 runs in CTC mode.
 * • The Compare Match A interrupt routine displays a counter (4-bits binary number), by
 *      the four LEDs (0 = off, 1 = on). The speed by which the number is displayed is default
 *      approx. 4 times per second.
 * • The LEDs are connected as follows: Bit3: D9, Bit2: D8, Bit 1: D6, Bit 0: D5. With four
 *       bits we can count from 0x0 till 0xF Hex (0 till 15 decimal).
 * • Pressing button D10 yields an interrupt; the interrupt routine slows the update speed
 *      of the binary numbers down.
 * • Pressing button D11 yields an interrupt; the interrupt routine speeds up the update
 *      speed of the binary numbers.
 * */

#include <Arduino.h>

#define WHITE_LED_MODE DDRB                     /* ddr (Data Direction Register) register for led */
#define WHITE_LED_OUT PORTB                     /* port (output data) register for green led */
#define WHITE_LED_POS 1                         /* bitmask for pin B1 (9) */
#define WHITE_LED_MASK (1 << WHITE_LED_POS)     /* bitmask for pin B1 (9) */

#define RED_LED_MODE DDRB                       /* ddr (Data Direction Register) register for led */
#define RED_LED_OUT PORTB                       /* port (output data) register for green led */
#define RED_LED_POS 0                           /* bitmask for pin B0 (8) */
#define RED_LED_MASK (1 << RED_LED_POS)         /* bitmask for pin B0 (8) */

#define GREEN_LED_MODE DDRD                     /* ddr (Data Direction Register) register for led */
#define GREEN_LED_OUT PORTD                     /* port (output data) register for green led */
#define GREEN_LED_POS 6                         /* bitmask for pin D6 (6) */
#define GREEN_LED_MASK (1 << GREEN_LED_POS)     /* bitmask for pin D6 (6) */

#define YELLOW_LED_MODE DDRD                    /* ddr (Data Direction Register) register for led */
#define YELLOW_LED_OUT PORTD                    /* port (output data) register for green led */
#define YELLOW_LED_POS 5                        /* bitmask for pin D5 (5) */
#define YELLOW_LED_MASK (1 << YELLOW_LED_POS)   /* bitmask for pin D5 (5) */

#define BUILTIN_LED_MODE DDRB                   /* ddr (Data Direction Register) register for builtin led */
#define BUILTIN_LED_OUT PORTB                   /* port (output data) register for builtin led */
#define BUILTIN_LED_POS 5                       /* bit position for pin B5 (13) */
#define BUILTIN_LED_MASK (1 << BUILTIN_LED_POS) /* bitmask for pin B5 (13) */

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

volatile uint8_t counter = 0;               // 4-bit counter (0 to 15)
const uint16_t speed_up_factor = 10;        // Speed up by reducing OCR1A
const uint16_t slow_down_factor = 50;       // Slow down by increasing OCR1A
volatile uint8_t built_in_led_counter = 0;

uint32_t last_toggle_time = 0;      // For toggling the built-in LED at 1 Hz

void setup() {
    Serial.begin(9600);
    WHITE_LED_MODE |= WHITE_LED_MASK;       // set pin B1 as output
    RED_LED_MODE |= RED_LED_MASK;           // set pin B0 as output
    GREEN_LED_MODE |= GREEN_LED_MASK;       // set pin D6 as output
    YELLOW_LED_MODE |= YELLOW_LED_MASK;     // set pin D5 as output
    BUILTIN_LED_MODE |= BUILTIN_LED_MASK;   // set pin B7 as output

    WHITE_LED_OUT &= ~WHITE_LED_MASK;       // Initialize led to off
    RED_LED_OUT &= ~RED_LED_MASK;           // Initialize led to off
    GREEN_LED_OUT &= ~GREEN_LED_MASK;       // Initialize led to off
    YELLOW_LED_OUT &= ~YELLOW_LED_MASK;     // Initialize led to off
    cli();
    // Configure INT0 for the Green Button on falling edge
    EICRA |= (1 << ISC00);                  // Set ISC00 to 1
    EICRA &= ~(0 << ISC01);                 // Clear ISC01 to 0
    EIMSK |= (1 << INT0);                   // Enable INT0 interrupt

    // Configure INT1 for the Yellow Button on falling edge
    EICRA |= (1 << ISC10);                  // Set ISC10 to 1
    EICRA &= ~(0 << ISC11);                 // Clear ISC11 to 0
    EIMSK |= (1 << INT1);                   // Enable INT1 interrupt

    // initialize Timer1
    TCCR1A = 0;     // Set TCCR1A register to 0
    TCCR1B = 0;     // Set TCCR1B register to 0

    // Set compare match register to 4 times per second
    OCR1A = 2024;  // Sets the timer compare value

    // Turn on CTC mode
    TCCR1B |= (1 << WGM12);

    // Set CS10 and CS12 (16MHz / 1024)
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);

    // Enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei();
}

ISR(INT0_vect) {
    uint32_t now = millis();
    if (now - prev_ylw_btn > debounce_delay) {  // debounce logic
        // Slow down the timer by increasing OCR1A
        if (OCR1A < 65535 - slow_down_factor) {
            OCR1A += slow_down_factor;
            Serial.print("Decrease: ");
            Serial.print(OCR1A);
            Serial.print("\n");
        }
        prev_ylw_btn = now;                     // Update timestamp
        ylw_btn_state = !ylw_btn_state;
    }
}

ISR(INT1_vect) {
    uint32_t now = millis();
    if (now - prev_grn_btn > debounce_delay) {  // debounce logic
        // Speed up the timer by decreasing OCR1A
        if (OCR1A > speed_up_factor) {
            OCR1A -= speed_up_factor;
            Serial.print("Increase: ");
            Serial.print(OCR1A);
            Serial.print("\n");
        }
        prev_grn_btn = now;                     // Update timestamp
        grn_btn_state = !grn_btn_state;
    }
}

// Display the binary counter on LEDs
void display_counter() {
    //todo
    uint8_t lsb = (counter & 0b11) << YELLOW_LED_POS;   // Yellow and green led
    uint8_t msb = (counter & 0b1100) >> 2;              // Red and white led

    YELLOW_LED_OUT = ((YELLOW_LED_OUT & ~((0b11) << YELLOW_LED_POS)) | lsb);
    RED_LED_OUT = (RED_LED_OUT & ~(0b11)) | msb;
}

// Interrupt service routine for Timer1 Compare Match A (Counter and Built-in LED)
ISR(TIMER1_COMPA_vect) {
    counter++;
    if (counter > 15) {
        counter = 0;
    }
    built_in_led_counter++;
    // Toggle built-in LED at 1Hz (every 4 timer cycles)
    if (built_in_led_counter >= 4) {
        BUILTIN_LED_OUT ^= BUILTIN_LED_MASK;        // Toggle build in led
        built_in_led_counter = 0;
    }
    // Display the counter on LEDs
    display_counter();
}

void loop() {}