//
// Created by Lynn Meindertsma on 11 Nov 2024.
//

#include <Arduino.h>
#include <Wire.h>

#ifndef PROGRAMMING_I2C_BUS_BME280_HPP
#define PROGRAMMING_I2C_BUS_BME280_HPP

// Define oversampling and mode
#define BME280_OSRS_H_16X    0b101       /* Humidity oversampling x16 */
#define BME280_OSRS_T_16X    0b101       /* Temperature oversampling x16 */
#define BME280_OSRS_P_16X    0b101       /* Pressure oversampling x16 */
#define BME280_MODE_NORMAL   0b11        /* Normal mode */
#define BME280_CTRL_MEAS     ((BME280_OSRS_T_16X << 5) | (BME280_OSRS_P_16X << 2) | BME280_MODE_NORMAL)

class BME280{
public:
    void BME280_init(uint8_t ctrl_hum, uint8_t ctrl_meas);
    uint8_t BME280_GetID();                     //0x60 id, address 0xD0
    void BME280_Reset();
    uint8_t BME280_CtrlHum();
    void BME280_CtrlHum(uint8_t bit_pattern);
    uint8_t BME280_CtrlMeas();
    void BME280_CtrlMeas(uint8_t bit_pattern);
    void BME280_ReadData();
    uint16_t BME280_ReadHumidity();
    uint32_t BME280_ReadTemperature();
    uint32_t BME280_ReadPressure();

private:
    uint8_t data[8];
};



#endif //PROGRAMMING_I2C_BUS_BME280_HPP
