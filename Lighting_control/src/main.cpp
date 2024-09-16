#include <Arduino.h>

#define YELLOW_LED 6
#define button A5

// Define the number of buttons
const int num_buttons = 3;

// Track button press states (moved outside loop to maintain state across iterations)
bool button_pressed[num_buttons] = {false, false, false};

// Define the voltage ranges for each button
int button_thresholds[num_buttons][2] = {
    {169, 508},   // Range for button 0
    {508, 680},   // Range for button 1
    {680, 690},   // Range for button 2
};

bool led_state = LOW;   // To track the state of the LED
unsigned long led_on_time = 0;  // Time when the LED was turned on
bool led_on = false;    // Track if the LED is currently on

void setup() {
    Serial.begin(9600);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(button, INPUT);
}

void loop() {
    int button_read = analogRead(button);  // Read the analog input
    Serial.println(button_read);  // Debugging: print the analog value

    // Determine button press states
    for (int i = 0; i < num_buttons; i++) {
        if (button_read > button_thresholds[i][0] && button_read < button_thresholds[i][1]) {
            if (!button_pressed[i]) {
                Serial.print("Button ");        //test print
                Serial.print(i);                //test print
                Serial.println(" pressed!");    //test print
                button_pressed[i] = true;  // Mark this button as pressed
            }
        } else {
            button_pressed[i] = false; // Reset the button press state if not in range
        }
    }

    // Check if Button 1 is pressed
    if (button_pressed[0]) {
        // Button 1 logic: Toggle LED state
        if (!led_on) {
            // Turn LED on
            digitalWrite(YELLOW_LED, HIGH);
            led_on = true;
            led_on_time = millis(); // Record the time when LED was turned on
        } else {
            // Turn LED off
            digitalWrite(YELLOW_LED, LOW);
            led_on = false;
        }
        return;
    }

    if (button_pressed[1]) {
        // Button 2 logic: Turn LED on for 10 sec
        if (!led_on) {
            digitalWrite(YELLOW_LED, HIGH);
            led_on = true;
            led_on_time = millis(); // Record the time when LED was turned on
        }
        if (millis() - led_on_time >= 10000) {
            digitalWrite(YELLOW_LED, LOW);
            led_on = false;
        }
    }

    if (button_pressed[2]) {
        // Button 2 logic: Turn LED on for 1 minute
        if (!led_on) {
            digitalWrite(YELLOW_LED, HIGH);
            led_on = true;
            led_on_time = millis(); // Record the time when LED was turned on
        }
        if (millis() - led_on_time >= 60000) {
            digitalWrite(YELLOW_LED, LOW);
            led_on = false;
        }
    }
}