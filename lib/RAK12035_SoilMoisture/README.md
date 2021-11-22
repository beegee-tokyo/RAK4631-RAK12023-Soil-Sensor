| <center><img src="./assets/rakstar.jpg" alt="RAKstar" width=25%></center>  | ![RAKWireless](./assets/RAK-Whirls.png) | [![Build Status](https://github.com/RAKWireless/RAK13005-TLE7259-Library/workflows/RAK%20Library%20Build%20CI/badge.svg)](https://github.com/RAKWireless/RAK13005-TLE7259-Library/actions) |
| -- | -- | -- |

# RAK12023 Soil Moisture Sensor

Simple Arduino Library for the WisBlock I2C Soil Moisture Sensor version from [RAKwireless](https://docs.rakwireless.com/Product-Categories/WisBlock) which works really great and is ready to use with I2C.

[RAKwireless RAK12023/RAK12035 Soil Moisture Sensor](https://store.rakwireless.com/products/soil-moisture-sensor-rak12035)

# Documentation

* **[Product Repository](https://github.com/RAKWireless/RAK2035_SoilMoisture)** - Product repository for the RAKWireless RAK12023/RAK12035 Soil Moisture sensor.
* **[Documentation](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK12023/Overview/)** - Documentation and Quick Start Guide for the RAK12023/RAK12035 Soil Moisture sensor.

# Installation

In Arduino IDE open Sketch->Include Library->Manage Libraries then search for RAK12035.    

In PlatformIO open PlatformIO Home, switch to libraries and search for RAK12035. 
Or install the library project depend by adding 
```log
lib_deps =
  rakwireless/RAK12035_SoilMoisture
```
into **`platformio.ini`**

For manual installation download the archive, unzip it and place the RAK12035_SoilMoisture folder into the library directory.    
In Arduino IDE this is usually <arduinosketchfolder>/libraries/    
In PlatformIO this is usually <user/.platformio/lib>     


# Usage

The library provides an interface class, which allows communication to the RAK12035 Soil Moisture sensor over I2C. Check out the examples how to use the Soil Moisture Sensor.     
- [RAK12035_Soil_Humidity](./examples/RAK12035_Soil_Humidity) simply reads the soil moisture and temperature values and prints them over USB Serial
- [RAK12035_Calibration](./examples/RAK12035_Calibration) shows how to implement the sensor calibration
- [RAK12035_LowPower](./examples/RAK12035_LowPower) shows how to shut down and restart the sensor between measurements
- [RAK12035_Change_Address](./examples/RAK12035_Change_Address) shows how to change the I2C address of the sensor
- [RAK12035_RAK4631_LPWAN](./examples/RAK12035_RAK4631_LPWAN) is a complete example for the RAK4631 using the [WisBlock API](https://github.com/beegee-tokyo/WisBlock-API) :arrow_heading_up:. It sends the measured soil moisture, soil temperature and a flag for valid data in Cayenne LPP format over LPWAN to a LPWAN server.


## _IMPORTANT_
Before you use the Soil Moisture sensor, you need to calibrate it. Please install the example sketch, then first write down the sensor values while the sensor is in air. Then submerge the sensor in water (only to the white line on the sensor) and write down the sensor values when the sensor is in the water. The value measured in air corresponds to 0% RH, the value measured in water correspondends to 100% RH.

## This class provides the following methods:

**RAK12035(uint8_t addr)**
Constructor for Soil Moisture sensor interface. Optionally set sensor I2C address if different from default    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in        | addr | Alternative I2C address |
|  return |  | none  | 

## Version History

### v1.0.0
- Initial Release

## Methods

### Constructor RAK12035
Optionally set sensor I2C address if different from default    

**void begin(bool wait)**    
Initializes anything ... it does a reset.    
When used without parameter or parameter value is false then a waiting time of at least 1 second is expected to give the sensor
some time to boot up.    
Alternatively use true as parameter and the method waits for a second and returns after that.          
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | wait  | If true, wait 1 second after the sensor was reset  | 

**bool get_sensor_version(uint8_t *version)**    
Get Firmware Version. 0x22 means 2.2    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | version  | Pointer to the variable to write the version into  |
| return      | true if I2C communication succeeded, false if an error occured | 

**bool get_sensor_capacitance(uint16_t *capacitance)**    
Reads the capacitance from the sensor and writes it into the variable **`capacitance`** and returns true. If the I2C read command fails, it returns false and you should disregard the value and check the connection to the sensor. 
More moisture will give you higher reading. Normally all sensors give about 200 - 300 as value in free air at
3.3V supply.
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | capacitance  | Pointer to the variable to write the capacitance into  |
| return      | true if I2C communication succeeded, false if an error occured | 

**bool get_sensor_moisture(uint8_t *moisture)**    
Reads the capacitance from the sensor, converts it to % RH based on calibration values and writes it into the variable **`moisture`** and returns true. If the I2C read command fails, it returns false and you should disregard the value and check the connection to the sensor.     
More moisture will give you higher reading. Normally all sensors give about 200 - 300 as value in free air at
3.3V supply.    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | moisture  | Pointer to the variable to write the moisture value into  |
| return      | true if I2C communication succeeded, false if an error occured | 

**bool get_sensor_temperature(float *temperature)**
Read the Temperature Measurement. Temperature is measured by the thermistor on the tip of the sensor. Calculated absolute measurement accuracy is better than 2%. The temperature is in degrees Celsius with factor 10, so need to divide by 10 to get real value. If the I2C read command fails, it returns false and you should disregard the value and check the connection to the sensor.    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | temperature  | Pointer to the variable to write the temperature value into  |
| return      | true if I2C communication succeeded, false if an error occured | 

**bool set_sensor_addr(uint8_t addr)**
Change I2C address of the sensor to the provided address (1..127). Requires a reset after it in order for the new address to become effective. Method returns true if the new address is set successfully on the sensor.    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | addr  | The new I2C address  |
| return      | true if I2C communication succeeded, false if an error occured or if the I2C address is out of range| 

**bool set_i2c_addr(uint8_t addr)**
Change I2C address the class uses to access the sensor. Useful if one instance of the class is used for multiple sensors.    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | addr  | The new I2C address to be used |
| return      | true if I2C address was accepted, false if the I2C address is out of range| 

**uint8_t get_sensor_addr(void)**
Return current address of the Sensor. This function returns the I2C address the sensor class is using.    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| return      | current used sensor I2C address | 

**bool sensor_on(void)**
Powers up the sensor. Use this function in order to save power inbetween measurements. After power up the connection to the sensor is tested. If the I2C read command fails, it returns false and you should check the connection to the sensor.    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| return      | true if I2C communication succeeded, false if an error occured | 

**void sensor_sleep(void)**
Powers down the sensor. Use this function in order to save power inbetween measurements.    
Parameters:    
- NONE

**void set_zero_val(uint16_t zero_val)**
After a sensor calibration the measured capacity in air (dry sensor) must be written with this command. The value is used in the **`get_sensor_moisture`** sensor to calculate the % RH value.    
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | zero_val  | The calibration value for 0%  |

**void set_hundred_val(uint16_t hundred_val)**
After a sensor calibration the measured capacity in water (wet sensor) must be written with this command. The value is used in the **`get_sensor_moisture`** sensor to calculate the % RH value.
Parameters:    
| Direction | Name | Function | 
| --------- | ---- | -------- |
| in          | hundred_val  | The calibration value for 100%  |

**void reset(void)**
Performs a reset of the soil sensor.
Parameters:    
None     

## Examples
There are several examples in the examples folder.
- [RAK12035_Soil_Humidity](./examples/RAK12035_Soil_Humidity) simply reads the soil moisture and temperature values and prints them over USB Serial
- [RAK12035_Calibration](./examples/RAK12035_Calibration) shows how to implement the sensor calibration
- [RAK12035_LowPower](./examples/RAK12035_LowPower) shows how to shut down and restart the sensor between measurements
- [RAK12035_Change_Address](./examples/RAK12035_Change_Address) shows how to change the I2C address of the sensor
- [RAK12035_RAK4631_LPWAN](./examples/RAK12035_RAK4631_LPWAN) is a complete example for the RAK4631 using the [WisBlock API](https://github.com/beegee-tokyo/WisBlock-API) :arrow_heading_up:. It sends the measured soil moisture, soil temperature and a flag for valid data in Cayenne LPP format over LPWAN to a LPWAN server. Please check the [WisBlock API docs]((https://github.com/beegee-tokyo/WisBlock-API) :arrow_heading_up: how to setup the LPWAN credentials by AT commands or over BLE.    

Example setup for LoRaWAN, OTAA join mode, sending data every 60 minutes. DevEUI, AppEUI and AppKey in this example are random choosen and need to be changed to your setup.

```
AT+NWM=1
AT+NJM=1
AT+DEVEUI=1000000000000001
AT+APPEUI=AB00AB00AB00AB00
AT+APPKEY=AB00AB00AB00AB00AB00AB00AB00AB00
AT+SENDFREQ=3600
AT+JOIN=1,1,10,10
```