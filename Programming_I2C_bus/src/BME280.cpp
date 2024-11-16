//
// Created by Lynn Meindertsma on 11 Nov 2024.
//
#include "BME280.hpp"

#define BME280_ADDRESS 0x76     /* If SDO is connected to GND */
#define ID 0x60
#define RESET 0xB6

// Register addresses for the BME280 sensor
#define BME280_REG_ID 0xD0
#define BME280_REG_RESET 0xE0
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_CTRL_MEAS 0xF4

//defines of data registers (counts till)
#define BME280_DATA 0xF7            /* (...FE) */
#define BME280_REG_PRESS_MSB 0xF7   /* (...F9) */
#define BME280_REG_TEMP_MSB 0xFA    /* (...FC) */
#define BME280_REG_HUM_MSB 0xFD     /* (...FE) */


// Initialize sensor settings
void BME280::BME280_init(uint8_t ctrl_hum, uint8_t ctrl_meas) {
    BME280_Reset();
    delay(10);
    BME280_CtrlHum(ctrl_hum);
    BME280_CtrlMeas(ctrl_meas);
}
// Get the sensor ID
uint8_t BME280::BME280_GetID(){
    uint8_t id;
    // sensor ID, read quantity, register address, address size, send stop
    Wire.requestFrom(BME280_ADDRESS, 1, BME280_REG_ID, 1, 1);

    if (!Wire.available()){ return 255; }   // Error return
    id = Wire.read();              // Read a single byte from the I2C bus
    if (id != ID){ return 0; }     // Error return

    return id;
}
// Reset the sensor
void BME280::BME280_Reset() {
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(BME280_REG_RESET); // Register for reset
    Wire.write(RESET);             // Reset command
    Wire.endTransmission();
    delay(100);                  // Wait for reset
}
// Read CtrlHum register
uint8_t BME280::BME280_CtrlHum() {
    uint8_t ctrl_hum;
    // sensor ID, read quantity, register address, address size, send stop
    Wire.requestFrom(BME280_ADDRESS, 1, BME280_REG_CTRL_HUM, 1, 1);
    if (!Wire.available()){ return 255; }   // Error return
    ctrl_hum = Wire.read();             // Read a single byte from the I2C bus

    return ctrl_hum;
}
// Write CtrlHum register
void BME280::BME280_CtrlHum(uint8_t bit_pattern) {
    // Step 1: Write the provided bit pattern to the humidity control register (0xF2)
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(BME280_REG_CTRL_HUM); // Address of the humidity control register
    Wire.write(bit_pattern);          // Write the new bit pattern to the register
    Wire.endTransmission();

    // Step 2: Write to the ctrl_meas register to make the changes effective
    uint8_t ctrl_meas = BME280_CtrlMeas();  // Read the current control measurement register value
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(BME280_REG_CTRL_MEAS); // Address of the control measurement register
    Wire.write(ctrl_meas);            // Reapply the current value of the ctrl_meas register
    Wire.endTransmission();
}
// Read CtrlMeas register
uint8_t BME280::BME280_CtrlMeas() {
    uint8_t ctrl_meas;
    // sensor ID, read quantity, register address, address size, send stop
    Wire.requestFrom(BME280_ADDRESS, 1, BME280_REG_CTRL_MEAS, 1, 1);
    if (!Wire.available()){ return 255; }   // Error return
    ctrl_meas = Wire.read();                // Read a single byte from the I2C bus

    return ctrl_meas;
}
// Write CtrlMeas register
void BME280::BME280_CtrlMeas(uint8_t bit_pattern) {
    // Step 1: Write the humidity control register (0xF2) to make the changes effective
    uint8_t ctrl_hum = BME280_CtrlHum();  // Read the current control humidity register value
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(BME280_REG_CTRL_HUM); // Address of the humidity control register
    Wire.write(ctrl_hum);          // Write the new bit pattern to the register
    Wire.endTransmission();

    // Step 2: Write the provided bit pattern to the ctrl_meas register
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(BME280_REG_CTRL_MEAS); // Address of the control measurement register
    Wire.write(bit_pattern);            // Reapply the current value of the ctrl_meas register
    Wire.endTransmission();
}
// Read data registers
void BME280::BME280_ReadData() {
    // sensor ID, read quantity, register address, address size, send stop
    Wire.requestFrom(BME280_ADDRESS, 8, BME280_DATA, 1, 1);
    if (Wire.available() < 8 ){ printf("Error: insufficient data received\n"); return;}   // Error return
    Wire.readBytes(data, 8);    // Read the buffer from the I2C bus
}
// Read humidity register
uint16_t BME280::BME280_ReadHumidity() {
    uint16_t humidity;
    BME280_ReadData();
    humidity = data[7];         // Lsb
    humidity |= (data[6] << 8); // Msb
    return (humidity);
}
// Read temperature register
uint32_t BME280::BME280_ReadTemperature() {
    uint32_t temp;
    BME280_ReadData();
    temp = (data[5] >> 4);      // Xlsb
    temp |= (data[4] << 4);     // Lsb
    temp |= (data[3] << 12);    // Msb

    return temp;
}
// Read pressure register
uint32_t BME280::BME280_ReadPressure() {
    uint32_t pressure;
    BME280_ReadData();
    pressure = (data[2] >> 4);      // Xlsb
    pressure |= (data[1] << 4);     // Lsb
    pressure |= (data[0] << 12);    // Msb

    return pressure;
}
