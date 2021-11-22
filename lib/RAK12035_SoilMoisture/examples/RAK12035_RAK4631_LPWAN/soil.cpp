/**
 * @file soil.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Soil sensor initialization and readings
 * @version 0.1
 * @date 2021-08-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "app.h"

/** Sensor */
RAK12035 sensor;

soil_data_s g_soil_data;

struct calib_values_s
{
	uint16_t zero_val = 75;
	uint16_t hundred_val = 250;
};

calib_values_s calib_values;

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
using namespace Adafruit_LittleFS_Namespace;

static const char soil_name[] = "SOIL";

File soil_file(InternalFS);

bool init_soil(void)
{
	// Check if sensors is available
	bool found_sensor = false;
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	pinMode(WB_IO5, INPUT);

	Wire.begin();

	// Initialize the sensor
	sensor.begin();

	uint8_t data = 0;

	// Check the sensor version
	if (!sensor.get_sensor_version(&data))
	{
		MYLOG("SOIL", "No sensor found");
	}
	else
	{
		MYLOG("SOIL", "Sensor FW version %d", data);
		found_sensor = true;
	}

	read_calib();

	sensor.set_zero_val(calib_values.zero_val);
	sensor.set_hundred_val(calib_values.hundred_val);

	sensor.sensor_sleep();

	Wire.end();

	return found_sensor;
}

void read_soil(void)
{
	uint16_t sensTemp = 0;
	uint8_t sensHumid = 0;
	uint32_t avgTemp = 0;
	uint32_t avgHumid = 0;

	// Wake up the sensor
	Wire.begin();
	if (!sensor.sensor_on())
	{
		MYLOG("SOIL", "Can't wake up sensor");
		g_soil_data.temp_1 = 0xFF;
		g_soil_data.temp_2 = 0xFF;

		g_soil_data.humid_1 = 0xFF;

		g_soil_data.valid = 0;

		Wire.end();

		return;
	}

	// Get the sensor values
	bool got_value = false;
	for (int retry = 0; retry < 3; retry++)
	{
		if (sensor.get_sensor_moisture(&sensHumid) && sensor.get_sensor_temperature(&sensTemp))
		{
			got_value = true;
			retry = 4;
			avgTemp = sensTemp;
			avgHumid = sensHumid;
			for (int avg = 0; avg < 50; avg++)
			{
				if (sensor.get_sensor_temperature(&sensTemp))
				{
					avgTemp += sensTemp;
					avgTemp /= 2;
				}

				if (sensor.get_sensor_moisture(&sensHumid))
				{
					if (sensHumid != NAN)
					{
						avgHumid += sensHumid;
						avgHumid /= 2;
					}
				}
			}
		}
	}

	MYLOG("SOIL", "Sensor reading was %s", got_value ? "success" : "unsuccessful");
	MYLOG("SOIL", "T %.2f H %ld", (double)(avgTemp/10.0), avgHumid);

	if (g_ble_uart_is_connected)
	{
		g_ble_uart.printf("Sensor reading was %s\n", got_value ? "success" : "unsuccessful");
		g_ble_uart.printf("T %.2f H %ld\n", (double)(avgTemp / 10.0), avgHumid);
	}

	avgTemp = avgTemp * 10.0;
	avgHumid = avgHumid * 2.0;

	g_soil_data.temp_1 = (uint8_t)(avgTemp >> 8);
	g_soil_data.temp_2 = (uint8_t)(avgTemp);

	g_soil_data.humid_1 = (uint8_t)(avgHumid);

	if (got_value)
	{
		g_soil_data.valid = 1;
	}
	else
	{
		g_soil_data.valid = 0;
	}
	sensor.sensor_sleep();

	Wire.end();
}

uint16_t start_calib(bool is_dry)
{
	MYLOG("SOIL", "Starting calibration for %s", is_dry ? "dry" : "wet");
	Serial.flush();
	uint16_t new_reading = 0;
	uint16_t new_value = 0;
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, HIGH);

	// Stop app timer while we do calibration
	g_task_wakeup_timer.stop();

	Wire.begin();

	if (!sensor.sensor_on())
	{
		MYLOG("SOIL", "Can't wake up sensor");
		Wire.end();

		if (is_dry)
		{
			return calib_values.zero_val;
		}
		else
		{
			return calib_values.hundred_val;
		}
	}

	sensor.get_sensor_capacitance(&new_value);

	for (int readings = 0; readings < 100; readings++)
	{
		sensor.get_sensor_capacitance(&new_reading);
		new_value += new_reading;
		new_value = new_value / 2;
		delay(250);
		digitalToggle(LED_GREEN);
		digitalToggle(LED_BLUE);
	}

	bool need_save = false;

	// Check if needed to save calibration value
	if (is_dry)
	{
		if (calib_values.zero_val != new_value)
		{
			need_save = true;
		}
		// Save dry calibration value
		calib_values.zero_val = new_value;
		MYLOG("SOIL", "Dry calibration value %d", calib_values.zero_val);
	}
	else
	{
		if (calib_values.hundred_val != new_value)
		{
			need_save = true;
		}
		// Save wet calibration value
		calib_values.hundred_val = new_value;
		MYLOG("SOIL", "Wet calibration value %d", calib_values.hundred_val);
	}

	// Values changed, set in class and save them
	if (need_save)
	{
		save_calib();
		sensor.set_zero_val(calib_values.zero_val);
		sensor.set_hundred_val(calib_values.hundred_val);
	}

	if (g_lorawan_settings.send_repeat_time != 0)
	{
		// Now we are connected, start the timer that will wakeup the loop frequently
		g_task_wakeup_timer.stop();
		g_task_wakeup_timer.setPeriod(g_lorawan_settings.send_repeat_time);
		g_task_wakeup_timer.start();
	}

	// Return the result

	digitalWrite(LED_BLUE, LOW);
	digitalWrite(LED_GREEN, LOW);
	sensor.sensor_sleep();
	Wire.end();

	return new_value;
}

void save_calib(void)
{
	InternalFS.remove(soil_name);

	if (soil_file.open(soil_name, FILE_O_WRITE))
	{
		soil_file.write((uint8_t *)&calib_values, sizeof(calib_values_s));
		soil_file.flush();
		soil_file.close();

		MYLOG("SOIL", "Saved Dry Cal: %d Wet Cal: %d", calib_values.zero_val, calib_values.hundred_val);
	}
	else
	{
		MYLOG("SOIL", "Failed to save calibration values");
	}
}

void read_calib(void)
{
	MYLOG("SOIL", "Reading calibration data");

	// Check if file exists
	if (!soil_file.open(soil_name, FILE_O_READ))
	{
		MYLOG("SOIL", "File doesn't exist, create it");

		delay(100);
		// InternalFS.remove(soil_name);

		if (soil_file.open(soil_name, FILE_O_WRITE))
		{
			calib_values_s default_settings;
			soil_file.write((uint8_t *)&default_settings, sizeof(calib_values_s));
			soil_file.flush();
			soil_file.close();
		}
		soil_file.open(soil_name, FILE_O_READ);
	}
	soil_file.read((uint8_t *)&calib_values, sizeof(calib_values_s));
	soil_file.close();

	MYLOG("SOIL", "Got Dry Cal: %d Wet Cal: %d", calib_values.zero_val, calib_values.hundred_val);
}

uint16_t get_calib(bool is_dry)
{
	if (is_dry)
	{
		return calib_values.zero_val;
}
	return calib_values.hundred_val;
}