#include "BME280.hpp"
#include <Wire.h>

#define SLAVE_ADDRESS 0x42

void data_to_slave(uint8_t data);
BME280 bme280;
uint8_t data = 0;

void setup() {
    Wire.begin();                   // Join i2c bus (address optional for master)
    Serial.begin(9600);      // Start serial for output
    bme280.BME280_init( BME280_OSRS_H_16X, BME280_CTRL_MEAS);
}

uint8_t x = 0;

void loop() {

    Serial.print("Humidity:");
    Serial.print(bme280.BME280_ReadHumidity());
    delay(200);
    Serial.print(",");
    Serial.print("Temperature:");
    Serial.print(bme280.BME280_ReadTemperature());
    delay(200);
    Serial.print(",");
    Serial.print("Pressure:");
    Serial.println(bme280.BME280_ReadPressure());
    delay(200);

    data_to_slave(data);
    data++;
    delay(200);
}

void data_to_slave(uint8_t data) {
    Wire.beginTransmission(SLAVE_ADDRESS);  // Start communication with slave at address 0x42
    Wire.write(data);                       // Send the data to the slave
    Wire.endTransmission();                 // End transmission

    // Now request data from the slave
    Wire.requestFrom(SLAVE_ADDRESS, 1);     // Request 1 byte from the slave
    if (Wire.available()) {
        uint8_t response = Wire.read();        // Read the response from the slave
        Serial.print("Received from slave: ");
        Serial.println(response);
    }
}