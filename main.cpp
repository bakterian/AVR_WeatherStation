////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////    main.c
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

/* pre-compiled headers and globals */
#include "Configuration/Globals.h"

/* references to sensor drivers */
#include "drivers/sensors/DustSharpGp2y10/DustSensor.h"
#include "drivers/sensors/tempHumidityDHT22/DhtTalker.h"
#include "drivers/sensors/tempHumidityDHT22/TempHumiditySensor.h"
#include "drivers/sensors/LightSensorBH1750/LightSensor.h"

/* references to task classes */
#include "OS/Tasks/BlinkTask/BlinkTask.h"
#include "OS/Tasks/MeasurementTask/MeasurementTask.h"


	/* Main program loop */
	int main(void) __attribute__((OS_main));

	int main(void)
	{

		/* ------------------------- UTILS  ---------------------------- */
		/* Serial point handle - this global struct used in the interrupt*/
		xSerialPort = xSerialPortInitMinimal( USART0, 38400, portSERIAL_BUFFER_TX, portSERIAL_BUFFER_RX);

		/* Create the Mutex for accessing the console */
		xConsoleMutex = xSemaphoreCreateMutex();
		/* ------------------------------------------------------------- */

		/* ---------------- SENSOR CONFIGURATION  ---------------------- */

		/* -------------------------- DUST SENSOR ---------------------- */
		// TODO: move sensor pin and port definition to configuration level
		::drivers::sensors::ISensor::Configuration DUST_SENSOR_CFG =
		{
			PSTR("DustSensor"),	 				/* sensor description  */
			PSTR("ug/m3"),		  			   /*  sensor units	      */
			::drivers::sensors::eDustSensor   /*   sensor type	     */
		};

		::drivers::sensors::DustSensor DUST_SENSOR(DUST_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		/* ----------------  TEMPERATURE HUMIDITY SENSOR ---------------- */
		::drivers::sensors::DhtTalker DHT_TALKER;

		::drivers::sensors::TempHumiditySensor::DhtMeasurmentData DHT_MEAS_DATA = { 0U, 0U, 0U};

		::drivers::sensors::TempHumiditySensor::DhtConfiguartion HUMIDITY_SENSOR_CFG =
		{
			&DHT_TALKER,
			DHT_MEAS_DATA,
			{
				PSTR("HumiditySensor"),	 				/* sensor description  */
				PSTR("proc RH"),				   	   /*  sensor units	      */
				::drivers::sensors::eHumiditySensor   /*   sensor type	     */
			}
		};
		::drivers::sensors::TempHumiditySensor HUMIDITY_SENSOR(HUMIDITY_SENSOR_CFG);

		::drivers::sensors::TempHumiditySensor::DhtConfiguartion TEMP_SENSOR_CFG =
		{
			&DHT_TALKER,
			DHT_MEAS_DATA,
			{
				PSTR("TemperatureSensor"),				   /* sensor description  */
				PSTR("deg C"),					   	   	  /*  sensor units	      */
				::drivers::sensors::eTemperatureSensor   /*   sensor type	     */
			}
		};

		::drivers::sensors::TempHumiditySensor TEMPERATURE_SENSOR(TEMP_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		/* ----------------------  LIGHT SENSOR ---------------------- */

		const uint16_t I2C_ATMEGA_ADDRESS = uint16_t(0x64U);
		::drivers::sensors::I2cManager::Configuration I2C_MANAGER_CFG =
		{
			I2C_ATMEGA_ADDRESS,	 										// i2c address
			::drivers::sensors::I2cManager::Master						// i2c operation mode
		};

		::drivers::sensors::I2cManager I2C_MANAGER(I2C_MANAGER_CFG);

		::drivers::sensors::LightSensor::Configuration LIGHT_SENSOR_CFG =
		{
			::drivers::sensors::LightSensor::ContinousHighResMode,	 	// i2c address
			::drivers::sensors::LightSensor::AddressV1,	   				// i2c operation mode
			 &I2C_MANAGER,
			 {
				PSTR("LightSensor"),			 						// sensor description
				PSTR("lux"),											// sensor units
				::drivers::sensors::eLightSensor			  			// sensor type
			 }
		};

		::drivers::sensors::LightSensor LIGHT_SENSOR(LIGHT_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		::drivers::sensors::ISensor* SENSOR_LIST[] =
		{
			&DUST_SENSOR,
			&HUMIDITY_SENSOR,
			&TEMPERATURE_SENSOR,
			&LIGHT_SENSOR
		};

		::Application::SensorManagement::SensorManager::SensorConfiguration SENSOR_MGR_CFG =
		{
			&SENSOR_LIST[0],											// list containing ISensors
			sizeof(SENSOR_LIST)/sizeof(::drivers::sensors::ISensor*),	// number of elements in sensor list
			(4000 / portTICK_PERIOD_MS)								    // 4s need to pass from last result print
		};

		::Application::SensorManagement::SensorManager SENSOR_MGR(SENSOR_MGR_CFG);
		/* -------------------------------------------------------- */

		/* ----------- TASK CONFIGURATION  ---------------------- */
		const ::OS::Tasks::MeasurementTask::TaskConfiguration MeasurmentTaskCfg =
		{
			"MeasurementTask",
			TaskPrio_Low,
			(unsigned int) 192,
			(10000 / portTICK_PERIOD_MS),
			SENSOR_MGR
		};
		::OS::Tasks::MeasurementTask cMeasTask(MeasurmentTaskCfg);

		const ::OS::Tasks::BlinkTask::TaskConfiguration BlinkTaskCfg =
		{
			"BlinkTask",
			TaskPrio_Low,
			(unsigned int) 128,
			(unsigned int) 500,
			(10000 / portTICK_PERIOD_MS)
		};
		::OS::Tasks::BlinkTask cBlinkTask(BlinkTaskCfg);

		/* -------------------------------------------------------- */
		//TODO move all description strings to progmem as currently they are stored on the stack -> Data -> RAM
		avrSerialPrintf_P( PSTR("\r\nFree Heap Size: %u\r\n"),xPortGetFreeHeapSize() );
		vTaskStartScheduler();
		avrSerialxPrint_P( &xSerialPort, PSTR("\r\nGoodbye... no space for idle task!\r\n")); // Doh, so we're dead...
	}
