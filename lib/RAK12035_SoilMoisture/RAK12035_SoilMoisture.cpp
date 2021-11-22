/*----------------------------------------------------------------------*
 * I2CSoilMoistureSensor.cpp - Arduino library for the Sensor version of*
 * I2C Soil Moisture Sensor version from Chrirp                         *
 * (https://github.com/Miceuz/i2c-moisture-sensor).                     *
 *                                                                      *
 * Ingo Fischer 11Nov2015                                               *
 * https://github.com/Apollon77/I2CSoilMoistureSensor                   *
 *                                                                      *
 * MIT license                                                          *
 *----------------------------------------------------------------------*/

#include "RAK12035_SoilMoisture.h"

//define release-independent I2C functions
#if defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#include <TinyWireM.h>
#define i2cBegin TinyWireM.begin
#define i2cBeginTransmission TinyWireM.beginTransmission
#define i2cEndTransmission TinyWireM.endTransmission
#define i2cRequestFrom TinyWireM.requestFrom
#define i2cRead TinyWireM.receive
#define i2cWrite TinyWireM.send
#elif ARDUINO >= 100
#include <Wire.h>
#define i2cBegin Wire.begin
#define i2cBeginTransmission Wire.beginTransmission
#define i2cEndTransmission Wire.endTransmission
#define i2cRequestFrom Wire.requestFrom
#define i2cRead Wire.read
#define i2cWrite Wire.write
#else
#include <Wire.h>
#define i2cBegin Wire.begin
#define i2cBeginTransmission Wire.beginTransmission
#define i2cEndTransmission Wire.endTransmission
#define i2cRequestFrom Wire.requestFrom
#define i2cRead Wire.receive
#define i2cWrite Wire.send
#endif

/*----------------------------------------------------------------------*
 * Constructor.                                                         *
 * Optionally set sensor I2C address if different from default          *
 *----------------------------------------------------------------------*/
RAK12035::RAK12035(uint8_t addr) : _sensorAddress(addr)
{
	// nothing to do ... Wire.begin needs to be put outside of class
}

/*----------------------------------------------------------------------*
 * Initializes anything ... it does a reset.                            *
 * When used without parameter or parameter value is false then a       *
 * waiting time of at least 1 second is expected to give the sensor     *
 * some time to boot up.                                                *
 * Alternatively use true as parameter and the method waits for a       *
 * second and returns after that.                                       *
 *----------------------------------------------------------------------*/
void RAK12035::begin(bool wait)
{
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);

	// Reset the sensor
	reset();

	delay(500);

	time_t timeout = millis();
	uint8_t data;
	while (!get_sensor_version(&data))
	{
		if ((millis() - timeout) > 5000)
		{
			return;
		}
	}
	delay(500);
}

/**
 * @brief Get the sensor firmware version
 * 
 * @param version The sensor firmware version
 * @return true I2C transmission success
 * @return false I2C transmission failed
 */
bool RAK12035::get_sensor_version(uint8_t *version)
{
	return read_rak12035(SOILMOISTURESENSOR_GET_VERSION, version, 1);
}

/**
 * @brief Get the sensor moisture value as capacitance value
 * 
 * @param capacitance Variable to store value
 * @return true I2C transmission success
 * @return false I2C transmission failed
 */
bool RAK12035::get_sensor_capacitance(uint16_t *capacitance)
{
	uint8_t data[2] = {0};
	bool result = read_rak12035(SOILMOISTURESENSOR_GET_CAPACITANCE, data, 2);
	capacitance[0] = (((uint16_t)data[0]) << 8) | ((uint16_t)data[1]);
	return result;
}

/**
 * @brief Get the sensor moisture value as percentage
 * 
 * @param moisture Variable to store the value
 * @return true I2C transmission success
 * @return false I2C transmission failed
 */
bool RAK12035::get_sensor_moisture(uint8_t *moisture)
{
	float steps = 100.0 / (float)(_hundred_val - _zero_val);
	uint16_t capacitance = 0;
	if (get_sensor_capacitance(&capacitance))
	{
		moisture[0] = (uint8_t)((capacitance - _zero_val) * steps);
		if (moisture[0] > 100)
		{
			moisture[0] = 100;
		}
		return true;
	}
	return false;
}

/**
 * @brief Get the sensor temperature
 * 
 * @param temperature as uint16_t value * 10
 * @return true I2C transmission success
 * @return false I2C transmission failed
 */
bool RAK12035::get_sensor_temperature(uint16_t *temperature)
{
	uint8_t data[2] = {0};
	bool result = read_rak12035(SOILMOISTURESENSOR_GET_TEMPERATURE, data, 2);
	temperature[0] = (((uint16_t)data[0]) << 8) | ((uint16_t)data[1]);
	return result;
}

/**
 * @brief Get the current I2C address from the sensor class
 * 
 * @return the address the sensor class is using
 */
uint8_t RAK12035::get_sensor_addr(void)
{
	return _sensorAddress;
}

/**
 * @brief Set the new I2C address the sensor class will use.
 * 
 * @param addr The new sensor address
 * @return false if the I2C address is invalid (only 1 to 127 is allowed)
 */
bool RAK12035::set_i2c_addr(uint8_t addr)
{
	if ((addr < 1) || (addr > 127))
	{
		return false;
	}
	_sensorAddress = addr;
	return true;
}

/**
 * @brief Set the new I2C address on the sensor. Requires a sensor reset after changing.
 * 
 * @param addr The new sensor address
 * @return true I2C transmission success
 * @return false I2C transmission failed or if the I2C address is invalid (only 1 to 127 is allowed)
 */
bool RAK12035::set_sensor_addr(uint8_t addr)
{
	if ((addr < 1) || (addr > 127))
	{
		return false;
	}
	if (write_rak12035(SOILMOISTURESENSOR_SET_I2C_ADDRESS, addr))
	{
		_sensorAddress = addr;
		// Reset the sensor
		reset();
		return true;
	}
	return false;
}

/**
 * @brief Enable the power supply to the sensor
 * 
 */
bool RAK12035::sensor_on(void)
{
	uint8_t data;
	digitalWrite(WB_IO2, HIGH);
	digitalWrite(WB_IO4, HIGH);
	// reset();
	time_t timeout = millis();
	while (!get_sensor_version(&data))
	{
		if ((millis() - timeout) > 5000)
		{
			return false;
		}
	}
	delay(500);
	return true;
}

/**
 * @brief Switch power supply of the sensor off
 * 
 */
void RAK12035::sensor_sleep(void)
{
	digitalWrite(WB_IO4, LOW);
	digitalWrite(WB_IO2, LOW);
}

/**
 * @brief Set the dry value from the sensor calibration
 * 
 * @param zero_val dry value
 */
void RAK12035::set_zero_val(uint16_t zero_val)
{
	_zero_val = zero_val;
}

/**
 * @brief Set the wet value from the sensor calibration
 * 
 * @param hundred_val wet value
 */
void RAK12035::set_hundred_val(uint16_t hundred_val)
{
	_hundred_val = hundred_val;
}

/**
 * @brief Reset the sensor by pulling the reset line low.
 * 
 */
void RAK12035::reset(void)
{
	pinMode(WB_IO4, OUTPUT);
	digitalWrite(WB_IO4, LOW);
	delay(500);
	digitalWrite(WB_IO4, HIGH);
}

/**
 * @brief I2C read from sensor
 * 
 * @param reg Sensor register to read
 * @param data Pointer to data buffer
 * @param length Number of bytes to read
 * @return true I2C transmission success
 * @return false I2C transmission failed
 */
bool RAK12035::read_rak12035(uint8_t reg, uint8_t *data, uint8_t length)
{
	i2cBeginTransmission(_sensorAddress);
	i2cWrite(reg);						   // sends five bytes
	uint8_t result = i2cEndTransmission(); // stop transmitting
	if (result != 0)
	{
		return false;
	}
	delay(20);
	i2cRequestFrom(_sensorAddress, length);
	int i = 0;
	time_t timeout = millis();
	while (Wire.available()) // slave may send less than requested
	{
		data[i++] = i2cRead(); // receive a byte as a proper uint8_t
		if ((millis()-timeout) > 1000)
		{
			break;
		}
	}
	if (i != length)
	{
		return false;
	}
	return true;
}

/**
 * @brief I2C write to the sensor
 * 
 * @param reg Register to write to
 * @param data Data to write
 * @return true I2C transmission success
 * @return false I2C transmission failed
 */
bool RAK12035::write_rak12035(uint8_t reg, uint8_t data)
{
	i2cBeginTransmission(_sensorAddress);
	i2cWrite(reg); // sends bytes
	i2cWrite(data);
	uint8_t result = i2cEndTransmission(); // stop transmitting
	return (result == 0) ? true : false;
}
