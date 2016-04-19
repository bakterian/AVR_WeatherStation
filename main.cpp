////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////    main.c
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

/* pre-compiled headers and globals */
#include "Configuration/Globals.h"
#include "drivers/sensors/DustSharpGp2y10/DustSensor.h"

/* references to task classes */
#include "OS/Tasks/BlinkTask/BlinkTask.h"
#include "OS/Tasks/MeasurementTask/MeasurementTask.h"

	/* Main program loop */
	int main(void) __attribute__((OS_main));

	int main(void)
	{
		/* -------------------- UTILS  ---------------------------- */
		/* Serial point handle - this global struct is used in the interrupt*/
		xSerialPort = xSerialPortInitMinimal( USART0, 38400, portSERIAL_BUFFER_TX, portSERIAL_BUFFER_RX);

	    /* Create the Mutex for accessing the console */
		xConsoleMutex = xSemaphoreCreateMutex();
		/* -------------------------------------------------------- */

		/* ----------- SENSOR CONFIGURATION  ---------------------- */

		::drivers::sensors::ISensor::Configuration DUST_SENSOR_CFG =
		{
			(const uint8_t*) "DustSensor",
			(const uint8_t*) "ug/m3",
			::drivers::sensors::eDustSensor
		};

		::drivers::sensors::DustSensor DUST_SENSOR(DUST_SENSOR_CFG);

		::drivers::sensors::DustSensor SENSOR_LIST[] =
		{
			DUST_SENSOR
		};


		::Application::SensorManagement::SensorManager::SensorConfiguration SENSOR_MGR_CFG =
		{
			&SENSOR_LIST[0],
			sizeof(SENSOR_LIST)/sizeof(::drivers::sensors::ISensor)
		};

		::Application::SensorManagement::SensorManager SENSOR_MGR(SENSOR_MGR_CFG);
		/* -------------------------------------------------------- */

		/* ----------- TASK CONFIGURATION  ---------------------- */
		const ::OS::Tasks::MeasurementTask::TaskConfiguration MeasurmentTaskCfg =
		{
			"MeasurementTask",
			TaskPrio_Low,
			(unsigned int) 256,
			SENSOR_MGR
		};
		::OS::Tasks::MeasurementTask cMeasTask(MeasurmentTaskCfg);


		const ::OS::Tasks::BlinkTask::TaskConfiguration BlinkTaskCfg =
		{
			"BlinkTask",
			TaskPrio_Low,
			(unsigned int) 256,
			(unsigned int) 500,
		};
		::OS::Tasks::BlinkTask cBlinkTask(BlinkTaskCfg);
		/* -------------------------------------------------------- */

		vTaskStartScheduler();

		avrSerialxPrint_P( &xSerialPort, PSTR("\r\n\n\nGoodbye... no space for idle task!\r\n")); // Doh, so we're dead...
	}
