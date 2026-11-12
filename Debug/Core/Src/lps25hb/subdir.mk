################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/lps25hb/lps25hb.c 

OBJS += \
./Core/Src/lps25hb/lps25hb.o 

C_DEPS += \
./Core/Src/lps25hb/lps25hb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/lps25hb/%.o Core/Src/lps25hb/%.su Core/Src/lps25hb/%.cyclo: ../Core/Src/lps25hb/%.c Core/Src/lps25hb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-lps25hb

clean-Core-2f-Src-2f-lps25hb:
	-$(RM) ./Core/Src/lps25hb/lps25hb.cyclo ./Core/Src/lps25hb/lps25hb.d ./Core/Src/lps25hb/lps25hb.o ./Core/Src/lps25hb/lps25hb.su

.PHONY: clean-Core-2f-Src-2f-lps25hb

