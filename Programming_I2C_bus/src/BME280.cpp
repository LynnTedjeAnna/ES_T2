//
// Created by Lynn Meindertsma on 11 Nov 2024.
//
#include "BME280.hpp"

#define BME280_ADDRESS 0x76     // If SDO is connected to GND
#define ID 0x60
#define ID_ADDRESS 0xD0
#define RESET 0xB6
#define RESET_ADDRESS 0xE0

uint8_t BME280::BME280_GetID(){
    uint8_t ID_received;
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.requestFrom(ID_ADDRESS, 1);

    if (!Wire.available()){ return 255; }   // Error return
    ID_received = Wire.read();              // Read a single byte from the I2C bus
    if (ID_received != ID){ return 0; }     // Error return

    Wire.endTransmission();
    return ID_received;
}

void BME280::BME280_Reset() {
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(RESET_ADDRESS);              // Register within BME280 to write to
    Wire.write(RESET);                      // The data to be written
    Wire.endTransmission();
}

uint8_t BME280::BME280_CtrlHum() {
    return 0;
}

void BME280::BME280_CtrlHum(uint8_t bit_pattern) {

}

uint8_t BME280::BME280_CtrlMeas() {
    return 0;
}

void BME280::BME280_CtrlMeas(uint8_t bit_pattern) {

}

long BME280::BME280_ReadTemperature() {
    return 0;
}

int BME280::BME280_ReadHumidity() {
    return 0;
}

long BME280::BME280_ReadPressure() {
    return 0;
}
