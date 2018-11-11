################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include drivers/subdir.mk
-include CMSIS/EFM32GG/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: bambi_hf_final.axf

# Tool invocations
bambi_hf_final.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "bambi_hf_final.ld" -Xlinker --gc-sections -Xlinker -Map="bambi_hf_final.map" --specs=nano.specs -o bambi_hf_final.axf "./CMSIS/EFM32GG/startup_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./drivers/segmentlcd.o" "./emlib/em_cmu.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_lcd.o" "./emlib/em_system.o" "./src/display.o" "./src/game_logic.o" "./src/input.o" "./src/main.o" "./src/segmentlcd_individual.o" "./src/timing.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: bambi_hf_final.hex'
	arm-none-eabi-objcopy -O ihex "bambi_hf_final.axf" "bambi_hf_final.hex"
	@echo ' '

	@echo 'Building bin file: bambi_hf_final.bin'
	arm-none-eabi-objcopy -O binary "bambi_hf_final.axf" "bambi_hf_final.bin"
	@echo ' '

	@echo 'Building s37 file: bambi_hf_final.s37'
	arm-none-eabi-objcopy -O srec "bambi_hf_final.axf" "bambi_hf_final.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "bambi_hf_final.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) bambi_hf_final.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
