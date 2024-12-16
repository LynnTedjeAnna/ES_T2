//-------------MASTER----------------------------------------------------------------------------------------------//
#include <Arduino.h>
#include <SoftwareSerial.h>

#define RED_LED 13
#define YELLOW_LED 12
#define GREEN_LED 11

uint32_t prev = 0;
uint32_t green_duration = 10000;   // 10 seconds for green light
uint32_t yellow_duration = 3000;   // 3 seconds for yellow light

enum Traffic_state {
    GREEN = 'G',
    YELLOW = 'Y',
    RED = 'R'
};
Traffic_state current_state;

SoftwareSerial s_serial(2, 3, 100);  // RX on pin 2, TX on pin 3

uint8_t send_command_to_slave(Traffic_state command, uint32_t timeout);
Traffic_state receive_command_from_master();
void handle_communication_error();
void set_state(Traffic_state state);


void setup() {
    Serial.begin(9600);               // For debugging
    s_serial.begin(9600);

    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    set_state(RED);
    send_command_to_slave(RED, -1);  // wait indefinitely
    set_state(GREEN);
}

uint8_t send_command_to_slave(Traffic_state command, uint32_t timeout) {
    uint32_t start = millis();
    do {
        s_serial.write(command);
        delay(10);
    } while(!s_serial.available() && (millis() - start) < timeout);
    if (!s_serial.available() || s_serial.read() != command) {
        handle_communication_error();
        return 0;  // error
    }
    // todo: if loop error handler is called too often, clear the buffer here!
    return 1;  // success
}

Traffic_state receive_command_from_master() {
    while (1) {
        while (!s_serial.available());
        char s = (char)s_serial.read();
        if (s == RED || s == GREEN) {
            s_serial.write(s);
            return (Traffic_state)s;
        }
    }
}

void handle_communication_error() {
    set_state(RED);
    send_command_to_slave(RED, -1);  // wait indefinitely
    set_state(GREEN);
}

void set_state(Traffic_state state) {
    digitalWrite(RED_LED, state==RED);        // led on if state is RED, else off
    digitalWrite(YELLOW_LED, state==YELLOW);  // led on if state is YELLOW, else off
    digitalWrite(GREEN_LED, state==GREEN);    // led on if state is GREEN, else off
    current_state = state;
    prev = millis();
}


void loop() {
    uint32_t now = millis();

    if (s_serial.available() && current_state != RED) {
        handle_communication_error();
    }

    switch (current_state) {
    case GREEN:
        if (now - prev < green_duration) { break; }
        set_state(YELLOW);
        break;
    case YELLOW:
        if (now - prev < yellow_duration) { break; }
        if (!send_command_to_slave(GREEN, 1000)) { break; }
        set_state(RED);
        break;
    case RED:
        set_state(receive_command_from_master());
        break;
    default:
        Serial.print("How did we get here?");
        handle_communication_error();
        break;
    }
}


