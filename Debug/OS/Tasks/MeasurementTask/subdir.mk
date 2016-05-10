################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../OS/Tasks/MeasurementTask/MeasurementTask.cpp 

OBJS += \
./OS/Tasks/MeasurementTask/MeasurementTask.o 

CPP_DEPS += \
./OS/Tasks/MeasurementTask/MeasurementTask.d 


# Each subdirectory must supply rules for building sources it contributes
OS/Tasks/MeasurementTask/%.o: ../OS/Tasks/MeasurementTask/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"G:\3_PROGRAMOWANIE\8_C_AVR_Embeded\0_EclipseWorkspace\freeRTOS823_Static_Library\include" -Wall -O1 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -mcall-prologues -mrelax -funsigned-char -funsigned-bitfields -fno-exceptions -g -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


