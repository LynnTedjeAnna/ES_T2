//-------------MASTER----------------------------------------------------------------------------------------------//
#include <Arduino.h>
#include <cinttypes>
#include <SoftwareSerial.h>
#include "heartbeat.h"

#define RED_LED 13
#define YELLOW_LED 12
#define GREEN_LED 11

uint64_t previous_millis = 0;
uint32_t green_duration = 10000;   // 10 seconds for green light
uint32_t yellow_duration = 3000;   // 3 seconds for yellow light
uint32_t red_duration = 10000;     // 10 seconds for red light

// Create an instance of the HeartBeat class
HeartBeat heartbeat;

enum Traffic_state { GREEN, YELLOW, RED };
Traffic_state current_state;

SoftwareSerial s_serial(2, 3, 100);  // RX on pin 2, TX on pin 3
void handle_communication_error();

void setup() {
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    current_state = GREEN;
    digitalWrite(GREEN_LED, HIGH);   // Start with the Green light ON for the Master

    s_serial.begin(9600);
    heartbeat.begin(7, 1.0);          // Heartbeat on pin 7 with 1Hz frequency
    heartbeat.enable();
}

void send_command_to_slave(char command) {
    bool command_acknowledged = false;
    int retries = 3;  // Allow 3 retries for failed communication

    while (!command_acknowledged && retries > 0) {
        s_serial.write(command);
        delay(100);                // Give some time for the slave to respond

        if (s_serial.available()) {
            char response = s_serial.read();
            if (response == 'A') {
                command_acknowledged = true;
            } else {
                retries--;  // Reduce the number of retries if no ACK received
            }
        } else {
            retries--;  // Retry if no response from slave
        }
    }

    if (!command_acknowledged) {
        handle_communication_error(); // Go to a safe state after retries fail
    }
}

void handle_communication_error() {
    // Resend the command or go into a safe state
    send_command_to_slave('R');  // Send RED command to slave to ensure it's safe
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);

    // Attempt to resynchronize when communication is restored
    bool connection_restored = false;
    while (!connection_restored) {
        delay(1000);  // Wait for 1 second before retrying
        s_serial.write('R');  // Request slave to confirm its state

        if (s_serial.available()) {
            char response = s_serial.read();
            if (response == 'A') {
                connection_restored = true;

                digitalWrite(RED_LED, LOW);
                digitalWrite(GREEN_LED, HIGH);
                current_state = GREEN;
                previous_millis = millis();
            }
        }
    }
}


void loop() {
    uint64_t current_millis = millis();
    heartbeat.beat();                    // Continuously send the heartbeat pulse on pin 7

    // Check for communication from the slave
    if (s_serial.available()) {
        char msg = s_serial.read();
        if (msg == 'E') {
            handle_communication_error();
        }
    }

    // Traffic light state management for Master controlling its schedule
    switch (current_state) {
        case GREEN:
            if (current_millis - previous_millis >= green_duration) {
                send_command_to_slave('R');             // Tell the slave to switch to Red
                digitalWrite(RED_LED, LOW);
                digitalWrite(YELLOW_LED, HIGH);
                digitalWrite(GREEN_LED, LOW);
                current_state = YELLOW;
                previous_millis = current_millis;
            }
            break;

        case YELLOW:
            if (current_millis - previous_millis >= yellow_duration) {
                send_command_to_slave('G');             // Tell the slave to remain Green
                digitalWrite(RED_LED, HIGH);
                digitalWrite(YELLOW_LED, LOW);
                digitalWrite(GREEN_LED, LOW);
                current_state = RED;
                previous_millis = current_millis;
            }
            break;

        case RED:
            if (current_millis - previous_millis >= red_duration) {
                send_command_to_slave('Y');             // Tell the slave to switch to Yellow
                digitalWrite(RED_LED, LOW);
                digitalWrite(YELLOW_LED, LOW);
                digitalWrite(GREEN_LED, HIGH);
                current_state = GREEN;
                previous_millis = current_millis;
            }
            break;
    }
}


