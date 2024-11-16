#include "BME280.hpp"

BME280 bme280;

void setup() {
    Wire.begin();               // join i2c bus (address optional for master)
    Serial.begin(9600);   // start serial for output
    bme280.BME280_init( BME280_OSRS_H_16X, BME280_CTRL_MEAS);
}

void loop() {

    Serial.print("Humidity:");
    Serial.print(bme280.BME280_ReadHumidity());
    Serial.print(",");
    Serial.print("Temperature:");
    Serial.print(bme280.BME280_ReadTemperature());
    Serial.print(",");
    Serial.print("Pressure:");
    Serial.println(bme280.BME280_ReadPressure());

}

