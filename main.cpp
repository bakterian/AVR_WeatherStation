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
#include "drivers/sensors/PressureSensorBMP180/PressureSensor.h"

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

		::drivers::sensors::ISensor::MeasurementDataInfo DUST_MEAS_DATA_INFO =
		{
			PSTR("GP2Y10 Dust"),	 		   		//  measurement data description
			PSTR("ug/m3"),		  			   		//  measurement data units
			::drivers::sensors::Dust		   		//  measurement data type
		};

		::drivers::sensors::ISensor::Configuration DUST_SENSOR_CFG =
		{
			&DUST_MEAS_DATA_INFO,
			sizeof(DUST_MEAS_DATA_INFO)/sizeof(::drivers::sensors::ISensor::MeasurementDataInfo)
		};

		::drivers::sensors::DustSensor DUST_SENSOR(DUST_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		/* ----------------  TEMPERATURE HUMIDITY SENSOR ---------------- */
		::drivers::sensors::DhtTalker DHT_TALKER;

		::drivers::sensors::TempHumiditySensor::DhtMeasurmentData DHT_MEAS_DATA = { 0U, 0U, 0U};

		::drivers::sensors::ISensor::MeasurementDataInfo DHT22_HUMIDITY_MEAS_DATA_INFO =
		{
			PSTR("DHT22 Humidity"),	 			    //  measurement data description
			PSTR("proc RH"),		  			    //  measurement data units
			::drivers::sensors::Humidity			//  measurement data type
		};

		::drivers::sensors::ISensor::MeasurementDataInfo DHT22_TEMPERATURE_MEAS_DATA_INFO =
		{
			PSTR("DHT22 Temperature"),	 			//  measurement data description
			PSTR("deg C"),		  			   		//  measurement data units
			::drivers::sensors::Temperature			//  measurement data type
		};

		::drivers::sensors::ISensor::MeasurementDataInfo DHT22_MEAS_DATA_INFO_LIST[] =
		{
			DHT22_HUMIDITY_MEAS_DATA_INFO,
			DHT22_TEMPERATURE_MEAS_DATA_INFO
		};

		::drivers::sensors::ISensor::Configuration DHT22_SENSOR_BASE_CFG =
		{
			&DHT22_MEAS_DATA_INFO_LIST[0],
			sizeof(DHT22_MEAS_DATA_INFO_LIST)/sizeof(::drivers::sensors::ISensor::MeasurementDataInfo)
		};

		::drivers::sensors::TempHumiditySensor::DhtConfiguartion DHT22_SENSOR_CFG =
		{
			&DHT_TALKER,
			DHT_MEAS_DATA,
			DHT22_SENSOR_BASE_CFG
		};

		::drivers::sensors::TempHumiditySensor DHT22_TEMP_HUMIDITY_SENSOR(DHT22_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		/* ----------------------  LIGHT SENSOR ---------------------- */
		const uint16_t I2C_ATMEGA_ADDRESS = uint16_t(0x64U);

		::drivers::sensors::TwiManager::Configuration TWI_MANAGER_CFG =
		{
			I2C_ATMEGA_ADDRESS,	 										// i2c address
		};

		::drivers::sensors::TwiManager TWI_MANAGER(TWI_MANAGER_CFG);

		::drivers::sensors::ISensor::MeasurementDataInfo BH1750_MEAS_DATA_INFO =
		{
			PSTR("BH1750 Light"),	 		   		//  measurement data description
			PSTR("lux"),		  			   		//  measurement data units
			::drivers::sensors::Light		   		//  measurement data type
		};

		::drivers::sensors::ISensor::Configuration BH1750_SENSOR_CFG =
		{
			&BH1750_MEAS_DATA_INFO,
			sizeof(BH1750_MEAS_DATA_INFO)/sizeof(::drivers::sensors::ISensor::MeasurementDataInfo)
		};

		::drivers::sensors::LightSensor::Configuration LIGHT_SENSOR_CFG =
		{
			::drivers::sensors::LightSensor::ContinousHighResMode,	 	// i2c address
			::drivers::sensors::LightSensor::AddressV1,	   				// i2c operation mode
			 &TWI_MANAGER,
			 BH1750_SENSOR_CFG
		};

		::drivers::sensors::LightSensor LIGHT_SENSOR(LIGHT_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		/* -------------------- PRESSURE TEMP SENSOR -------------------- */
		::drivers::sensors::ISensor::MeasurementDataInfo BMP180_PRESSURE_MEAS_DATA_INFO =
		{
			PSTR("BMP Pressure"),	 		   		//  measurement data description
			PSTR("Pa"),		  			   		//  measurement data units
			::drivers::sensors::Pressure	   		//  measurement data type
		};

		::drivers::sensors::ISensor::MeasurementDataInfo BMP180_TEMP_MEAS_DATA_INFO =
		{
			PSTR("BMP Temperature"),	 		   	//  measurement data description
			PSTR("mDeg C"),		  			   		//  measurement data units
			::drivers::sensors::Temperature	   		//  measurement data type
		};

		::drivers::sensors::ISensor::MeasurementDataInfo BMP180_MEAS_DATA_INFO_LIST[] =
		{
			BMP180_PRESSURE_MEAS_DATA_INFO,
			BMP180_TEMP_MEAS_DATA_INFO
		};
		::drivers::sensors::ISensor::Configuration BMP180_SENSOR_BASE_CFG =
		{
			&BMP180_MEAS_DATA_INFO_LIST[0],
			sizeof(BMP180_MEAS_DATA_INFO_LIST)/sizeof(::drivers::sensors::ISensor::MeasurementDataInfo)
		};

		::drivers::sensors::PressureSensor::Configuration BMP180_SENSOR_CFG =
		{
			::drivers::sensors::PressureSensor::UltraHighResolution,	// BMP180 measurement mode
			 &TWI_MANAGER,												// Pointer to the Twi Manager
			 BMP180_SENSOR_BASE_CFG										// ISensor base configuration
		};

		::drivers::sensors::PressureSensor BMP180_PRESSURE_TEMP_SENSOR(BMP180_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		::drivers::sensors::ISensor* SENSOR_LIST[] =
		{
			&DUST_SENSOR,
			&DHT22_TEMP_HUMIDITY_SENSOR,
			&LIGHT_SENSOR,
			&BMP180_PRESSURE_TEMP_SENSOR
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
			(unsigned int) 224,
			(10000 / portTICK_PERIOD_MS),
			SENSOR_MGR
		};
		::OS::Tasks::MeasurementTask cMeasTask(MeasurmentTaskCfg);

		/*
		const ::OS::Tasks::BlinkTask::TaskConfiguration BlinkTaskCfg =
		{
			"BlinkTask",
			TaskPrio_Low,
			(unsigned int) 128,
			(unsigned int) 500,
			(10000 / portTICK_PERIOD_MS)
		};
		::OS::Tasks::BlinkTask cBlinkTask(BlinkTaskCfg);
		 */

		/* -------------------------------------------------------- */
		//TODO move all description strings to progmem as currently they are stored on the stack -> Data -> RAM
		avrSerialPrintf_P( PSTR("\r\nFree Heap Size: %u\r\n"),xPortGetFreeHeapSize() );
		vTaskStartScheduler();
		avrSerialxPrint_P( &xSerialPort, PSTR("\r\nGoodbye... no space for idle task!\r\n")); // Doh, so we're dead...
	}
