//
// Created by Lynn Meindertsma on 11 Nov 2024.
//

/*
 * Data readout is done by starting a burst read from 0xF7 to 0xFE (temperature, pressure and humidity).
 * The data are read out in an unsigned 20-bit format both for pressure and for temperature
 * and in an unsigned 16-bit format for humidity.
 * shadowing is performed in order to guarantee data consistency.
    If a new measurement is finished and the data registers are still being read, the new measurement
    results are transferred into shadow data registers. The content of shadow registers is transferred
    into data registers as soon as the user ends the burst read, even if not all data registers were
    read.
    The end of the burst read is marked by the rising edge of CSB pin in SPI case or by the recognition
    of a stop condition in I2C case. After the end of the burst read, all user data registers are updated
    at once.

 normal mode : 11
*/

#include <Arduino.h>
#include <Wire.h>

#ifndef PROGRAMMING_I2C_BUS_BME280_HPP
#define PROGRAMMING_I2C_BUS_BME280_HPP

class BME280{
public:
    uint8_t BME280_GetID();                     //0x60 id, address 0xD0
    void BME280_Reset();
    uint8_t BME280_CtrlHum();
    void BME280_CtrlHum(uint8_t bit_pattern);
    uint8_t BME280_CtrlMeas();
    void BME280_CtrlMeas(uint8_t bit_pattern);
    long BME280_ReadTemperature();
    int BME280_ReadHumidity();
    long BME280_ReadPressure();
};



#endif //PROGRAMMING_I2C_BUS_BME280_HPP
