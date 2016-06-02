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
			(const uint8_t*) "DustSensor",	 	/* sensor description  */
			(const uint8_t*) "ug/m3",		   /*  sensor units	      */
			::drivers::sensors::eDustSensor   /*   sensor type	     */
		};

		::drivers::sensors::DustSensor DUST_SENSOR(DUST_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		/* ----------------------  HUMIDITY SENSOR ---------------------- */
		::drivers::sensors::DhtTalker DHT_TALKER;

		::drivers::sensors::TempHumiditySensor::DhtMeasurmentData DHT_MEAS_DATA = { 0U, 0U, 0U};

		::drivers::sensors::TempHumiditySensor::DhtConfiguartion HUMIDITY_SENSOR_CFG =
		{
			&DHT_TALKER,
			DHT_MEAS_DATA,
			{
				(const uint8_t*) "HumiditySensor",	 	/* sensor description  */
				(const uint8_t*) "proc RH",		   	   /*  sensor units	      */
				::drivers::sensors::eHumiditySensor   /*   sensor type	     */
			}
		};
		::drivers::sensors::TempHumiditySensor HUMIDITY_SENSOR(HUMIDITY_SENSOR_CFG);

		::drivers::sensors::TempHumiditySensor::DhtConfiguartion TEMP_SENSOR_CFG =
		{
			&DHT_TALKER,
			DHT_MEAS_DATA,
			{
				(const uint8_t*) "TemperatureSensor",	   /* sensor description  */
				(const uint8_t*) "deg C",		   	   	  /*  sensor units	      */
				::drivers::sensors::eTemperatureSensor   /*   sensor type	     */
			}
		};

		::drivers::sensors::TempHumiditySensor TEMPERATURE_SENSOR(TEMP_SENSOR_CFG);
		/* -------------------------------------------------------------- */

		::drivers::sensors::ISensor* SENSOR_LIST[] =
		{
			&DUST_SENSOR,
			&HUMIDITY_SENSOR,
			&TEMPERATURE_SENSOR
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
			(unsigned int) 256,
			(10000 / portTICK_PERIOD_MS),
			SENSOR_MGR
		};
		::OS::Tasks::MeasurementTask cMeasTask(MeasurmentTaskCfg);

		const ::OS::Tasks::BlinkTask::TaskConfiguration BlinkTaskCfg =
		{
			"BlinkTask",
			TaskPrio_Low,
			(unsigned int) 256,
			(unsigned int) 500,
			(10000 / portTICK_PERIOD_MS)
		};
		::OS::Tasks::BlinkTask cBlinkTask(BlinkTaskCfg);

		/* -------------------------------------------------------- */

		vTaskStartScheduler();

		avrSerialxPrint_P( &xSerialPort, PSTR("\r\n\n\nGoodbye... no space for idle task!\r\n")); // Doh, so we're dead...
	}
