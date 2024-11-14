#include "BME280.hpp"

BME280 bme280;

void setup() {
    Wire.begin();        // join i2c bus (address optional for master)
    Serial.begin(9600);  // start serial for output
    bme280.BME280_Reset();  //test
    delay(200);
    Serial.println(bme280.BME280_GetID(), HEX); //test
}

void loop() {

}

