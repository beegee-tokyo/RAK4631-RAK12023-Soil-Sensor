/**
 * @file RAK12035_SoilMoisture.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Header file for Class RAK12035
 * @version 0.1
 * @date 2021-11-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef I2CSOILMOISTURESENSOR_H
#define I2CSOILMOISTURESENSOR_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Wire.h>

#define SLAVE_I2C_ADDRESS_DEFAULT 0x20
//Soil Moisture Sensor Register Addresses
#define SOILMOISTURESENSOR_GET_CAPACITANCE 0x01 // (r)   2 bytes
#define SOILMOISTURESENSOR_GET_I2C_ADDRESS 0x02 // (r)   1 bytes
#define SOILMOISTURESENSOR_SET_I2C_ADDRESS 0x03 // (w)   1 bytes
#define SOILMOISTURESENSOR_GET_VERSION 0x04		// (r)   1 bytes
#define SOILMOISTURESENSOR_GET_TEMPERATURE 0x05 // (r)   2 bytes

class RAK12035
{
public:
	RAK12035(uint8_t addr = SLAVE_I2C_ADDRESS_DEFAULT);

	void begin(bool wait = true);
	bool get_sensor_version(uint8_t *version);
	bool get_sensor_capacitance(uint16_t *capacitance);
	bool get_sensor_moisture(uint8_t *moisture);
	bool get_sensor_temperature(uint16_t *temperature);
	uint8_t get_sensor_addr(void);
	bool set_sensor_addr(uint8_t addr);
	bool set_i2c_addr(uint8_t addr);
	bool sensor_on(void);
	void sensor_sleep(void);
	void set_zero_val(uint16_t zero_val);
	void set_hundred_val(uint16_t hundred_val);
	void reset(void);

private:
	int _sensorAddress = SLAVE_I2C_ADDRESS_DEFAULT;
	uint16_t _zero_val = 200;
	uint16_t _hundred_val = 500;

	bool read_rak12035(uint8_t reg, uint8_t *data, uint8_t length);
	bool write_rak12035(uint8_t reg, uint8_t data);
};

#endif
