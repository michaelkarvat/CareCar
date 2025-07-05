################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/event_manager.c \
../Src/hx711.c \
../Src/hx711_parent.c \
../Src/main.c \
../Src/mhz19.c \
../Src/mhz19_usart1.c \
../Src/processing.c \
../Src/scheduler.c \
../Src/terminal.c \
../Src/timer2.c \
../Src/usart1.c \
../Src/usart2.c 

OBJS += \
./Src/event_manager.o \
./Src/hx711.o \
./Src/hx711_parent.o \
./Src/main.o \
./Src/mhz19.o \
./Src/mhz19_usart1.o \
./Src/processing.o \
./Src/scheduler.o \
./Src/terminal.o \
./Src/timer2.o \
./Src/usart1.o \
./Src/usart2.o 

C_DEPS += \
./Src/event_manager.d \
./Src/hx711.d \
./Src/hx711_parent.d \
./Src/main.d \
./Src/mhz19.d \
./Src/mhz19_usart1.d \
./Src/processing.d \
./Src/scheduler.d \
./Src/terminal.d \
./Src/timer2.d \
./Src/usart1.d \
./Src/usart2.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32 -DSTM32F3 -DDEBUG -DSTM32F303RETx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/event_manager.cyclo ./Src/event_manager.d ./Src/event_manager.o ./Src/event_manager.su ./Src/hx711.cyclo ./Src/hx711.d ./Src/hx711.o ./Src/hx711.su ./Src/hx711_parent.cyclo ./Src/hx711_parent.d ./Src/hx711_parent.o ./Src/hx711_parent.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/mhz19.cyclo ./Src/mhz19.d ./Src/mhz19.o ./Src/mhz19.su ./Src/mhz19_usart1.cyclo ./Src/mhz19_usart1.d ./Src/mhz19_usart1.o ./Src/mhz19_usart1.su ./Src/processing.cyclo ./Src/processing.d ./Src/processing.o ./Src/processing.su ./Src/scheduler.cyclo ./Src/scheduler.d ./Src/scheduler.o ./Src/scheduler.su ./Src/terminal.cyclo ./Src/terminal.d ./Src/terminal.o ./Src/terminal.su ./Src/timer2.cyclo ./Src/timer2.d ./Src/timer2.o ./Src/timer2.su ./Src/usart1.cyclo ./Src/usart1.d ./Src/usart1.o ./Src/usart1.su ./Src/usart2.cyclo ./Src/usart2.d ./Src/usart2.o ./Src/usart2.su

.PHONY: clean-Src

