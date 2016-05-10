################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../drivers/sensors/ISensor.cpp 

OBJS += \
./drivers/sensors/ISensor.o 

CPP_DEPS += \
./drivers/sensors/ISensor.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/sensors/%.o: ../drivers/sensors/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"G:\3_PROGRAMOWANIE\8_C_AVR_Embeded\0_EclipseWorkspace\freeRTOS823_Static_Library\include" -Wall -O1 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -mcall-prologues -mrelax -funsigned-char -funsigned-bitfields -fno-exceptions -g -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


