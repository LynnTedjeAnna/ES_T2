#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x42  // Define the I2C address for the slave device

uint8_t value = 0;  // Global variable to store the received value from the master

// Function prototypes
void respond_to_request();     // Callback to send data when requested by master
void respond_to_receive(int how_many);  // Callback to handle incoming data from the master

void setup() {
    Wire.begin(SLAVE_ADDRESS);        // Initialize as slave with address 0x42
    Wire.onReceive(respond_to_receive);  // Register the callback for receiving data from the master
    Wire.onRequest(respond_to_request);  // Register the callback for when the master requests data
    Serial.begin(9600);               // Start serial communication for debugging
}

void loop() {
    delay(100);  // Add a short delay to avoid CPU overload
}

// This function handles data received from the master
void respond_to_receive(int how_many) {
    // Check if there is data to read from the master
    if (Wire.available()) {
        value = Wire.read();  // Read a single byte of data from the master
        Serial.print("Received from master: ");
        Serial.println(value);  // Print the received byte to the serial monitor
    }
}

// This function responds to the master’s request for data
void respond_to_request() {
    // Check the received value and send an appropriate response
    if (value > 100) {
        Wire.write(2);  // Send 2 if the received value was greater than 100
    } else {
        Wire.write(4);  // Send 4 if the received value was less than or equal to 100
    }
}