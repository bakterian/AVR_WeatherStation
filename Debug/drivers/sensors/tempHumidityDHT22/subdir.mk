################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../drivers/sensors/tempHumidityDHT22/DhtTalker.cpp \
../drivers/sensors/tempHumidityDHT22/TempHumiditySensor.cpp 

OBJS += \
./drivers/sensors/tempHumidityDHT22/DhtTalker.o \
./drivers/sensors/tempHumidityDHT22/TempHumiditySensor.o 

CPP_DEPS += \
./drivers/sensors/tempHumidityDHT22/DhtTalker.d \
./drivers/sensors/tempHumidityDHT22/TempHumiditySensor.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/sensors/tempHumidityDHT22/%.o: ../drivers/sensors/tempHumidityDHT22/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"G:\3_PROGRAMOWANIE\8_C_AVR_Embeded\0_EclipseWorkspace\freeRTOS823_Static_Library\include" -Wall -g1 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -mcall-prologues -mrelax -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


