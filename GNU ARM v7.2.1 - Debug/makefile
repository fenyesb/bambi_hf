################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/EFM32GG/subdir.mk
-include BSP/subdir.mk
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
all: bambi_hf.axf

# Tool invocations
bambi_hf.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "bambi_hf.ld" -Xlinker --gc-sections -Xlinker -Map="bambi_hf.map" --specs=nano.specs -o bambi_hf.axf "./BSP/bsp_bcc.o" "./BSP/bsp_stk.o" "./BSP/bsp_stk_leds.o" "./CMSIS/EFM32GG/startup_gcc_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./emlib/em_assert.o" "./emlib/em_cmu.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_lcd.o" "./emlib/em_system.o" "./emlib/em_usart.o" "./src/main.o" "./src/segmentlcd.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: bambi_hf.hex'
	arm-none-eabi-objcopy -O ihex "bambi_hf.axf" "bambi_hf.hex"
	@echo ' '

	@echo 'Building bin file: bambi_hf.bin'
	arm-none-eabi-objcopy -O binary "bambi_hf.axf" "bambi_hf.bin"
	@echo ' '

	@echo 'Building s37 file: bambi_hf.s37'
	arm-none-eabi-objcopy -O srec "bambi_hf.axf" "bambi_hf.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "bambi_hf.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) bambi_hf.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
