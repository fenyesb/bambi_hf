################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/segmentlcd.c 

OBJS += \
./src/main.o \
./src/segmentlcd.o 

C_DEPS += \
./src/main.d \
./src/segmentlcd.d 


# Each subdirectory must supply rules for building sources it contributes
src/main.o: ../src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG=1' '-DEFM32GG990F1024=1' -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//hardware/kit/common/bsp" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//platform/emlib/inc" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//hardware/kit/EFM32GG_STK3700/config" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//platform/Device/SiliconLabs/EFM32GG/Include" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//platform/CMSIS/Include" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"src/main.d" -MT"src/main.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/segmentlcd.o: ../src/segmentlcd.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG=1' '-DEFM32GG990F1024=1' -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//hardware/kit/common/bsp" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//platform/emlib/inc" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//hardware/kit/EFM32GG_STK3700/config" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//platform/Device/SiliconLabs/EFM32GG/Include" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//platform/CMSIS/Include" -I"E:/Program Files (x86)/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.1//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"src/segmentlcd.d" -MT"src/segmentlcd.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


