################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include drivers/sensors/tempHumidityDHT22/subdir.mk
-include drivers/sensors/DustSharpGp2y10/subdir.mk
-include drivers/sensors/subdir.mk
-include OS/Tasks/MeasurementTask/subdir.mk
-include OS/Tasks/BlinkTask/subdir.mk
-include Configuration/subdir.mk
-include Application/SensorManagement/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
AvrWeatherStation.lss \

FLASH_IMAGE += \
AvrWeatherStation.hex \

EEPROM_IMAGE += \
AvrWeatherStation.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: AvrWeatherStation.elf secondary-outputs

# Tool invocations
AvrWeatherStation.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,AvrWeatherStation.map,--cref -mrelax -Wl,--gc-sections -L"G:\3_PROGRAMOWANIE\8_C_AVR_Embeded\0_EclipseWorkspace\freeRTOS823_Static_Library\Debug\libfreeRTOS823_Static_Library.a" -mmcu=atmega32 -o "AvrWeatherStation.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

AvrWeatherStation.lss: AvrWeatherStation.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S AvrWeatherStation.elf  >"AvrWeatherStation.lss"
	@echo 'Finished building: $@'
	@echo ' '

AvrWeatherStation.hex: AvrWeatherStation.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex AvrWeatherStation.elf  "AvrWeatherStation.hex"
	@echo 'Finished building: $@'
	@echo ' '

AvrWeatherStation.eep: AvrWeatherStation.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex AvrWeatherStation.elf  "AvrWeatherStation.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: AvrWeatherStation.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 AvrWeatherStation.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(S_DEPS)$(LSS)$(C_DEPS)$(FLASH_IMAGE)$(CC_DEPS)$(ELFS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(SIZEDUMMY)$(S_UPPER_DEPS)$(CPP_DEPS) AvrWeatherStation.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
