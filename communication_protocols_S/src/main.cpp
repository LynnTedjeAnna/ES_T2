//-------------SLAVE------------------------------------------------------------------------------------------------//
#include <Arduino.h>
#include <SoftwareSerial.h>

#define RED_LED 13
#define YELLOW_LED 12
#define GREEN_LED 11
#define hb_pin 7

unsigned long last_hb_time = 0;
unsigned long hb_timeout = 5000;  // Timeout: 5 seconds

SoftwareSerial s_serial(2, 3, 100);  // RX on pin 2, TX on pin 3

enum Traffic_state { GREEN, YELLOW, RED };
Traffic_state current_state = RED;  // Start with RED as opposite to the Master

void setup() {
    Serial.begin(9600);               // For debugging
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(hb_pin, INPUT);
    s_serial.begin(9600);

    digitalWrite(RED_LED, HIGH);      // Start with the RED light ON for the Slave
}

void handleLostConnection() {
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);  // Turn on Red light for safety
    s_serial.write('E');

    Serial.println("Lost connection, turned RED");        // Debugging output

    // Attempt recovery by waiting for the master's command
    while (true) {
        if (s_serial.available()) {
            char command = s_serial.read();
            if (command == 'R') {
                // Synchronize with the master's safe state (RED)
                digitalWrite(RED_LED, HIGH);
                s_serial.write('A');
                break;
            }
        }
        delay(500);  // Retry every 500ms
    }
}

void loop() {
    uint64_t current_millis = millis();

    // Check if the heartbeat signal is HIGH (pulse received)
    if (digitalRead(hb_pin) == HIGH) {
        last_hb_time = millis();

        Serial.println("Heartbeat detected!");  // Debugging output
    }

    // If more than 5 seconds have passed without a heartbeat, handle the lost connection
    if (current_millis - last_hb_time > hb_timeout) {
        handleLostConnection();
        last_hb_time = millis();
    }

    if (s_serial.available()) {
        char command = s_serial.read();

        Serial.print("Received command: "); // Debugging output
        Serial.println(command);

        // Turn off all lights before setting the new state
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);

        // Handle the state transitions based on the command received from the Master
        switch (command) {
            case 'G':  // Master is RED, Slave can turn GREEN
                digitalWrite(GREEN_LED, HIGH);
                current_state = GREEN;
                s_serial.write('A');  // Send ACK back to the master
                break;

            case 'Y':
                digitalWrite(YELLOW_LED, HIGH);
                current_state = YELLOW;
                s_serial.write('A');  // Send ACK back to the master
                break;

            case 'R':  // Master is GREEN, Slave must be RED
                digitalWrite(RED_LED, HIGH);
                current_state = RED;
                s_serial.write('A');  // Send ACK back to the master
                break;

            default:
                handleLostConnection();
                break;
        }
    }
}