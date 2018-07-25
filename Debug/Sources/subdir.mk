################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/Events.c \
../Sources/FIFO.c \
../Sources/FTM.c \
../Sources/Flash.c \
../Sources/HMI.c \
../Sources/LED.c \
../Sources/LPT.c \
../Sources/PIT.c \
../Sources/RTC.c \
../Sources/UART.c \
../Sources/main.c \
../Sources/measure.c \
../Sources/packet.c \
../Sources/selftest.c 

OBJS += \
./Sources/Events.o \
./Sources/FIFO.o \
./Sources/FTM.o \
./Sources/Flash.o \
./Sources/HMI.o \
./Sources/LED.o \
./Sources/LPT.o \
./Sources/PIT.o \
./Sources/RTC.o \
./Sources/UART.o \
./Sources/main.o \
./Sources/measure.o \
./Sources/packet.o \
./Sources/selftest.o 

C_DEPS += \
./Sources/Events.d \
./Sources/FIFO.d \
./Sources/FTM.d \
./Sources/Flash.d \
./Sources/HMI.d \
./Sources/LED.d \
./Sources/LPT.d \
./Sources/PIT.d \
./Sources/RTC.d \
./Sources/UART.d \
./Sources/main.d \
./Sources/measure.d \
./Sources/packet.d \
./Sources/selftest.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\99141145\Documents\develop\Project-john\Library" -I"C:/Users/99141145/Documents/develop/Project-john/Static_Code/IO_Map" -I"C:/Users/99141145/Documents/develop/Project-john/Sources" -I"C:/Users/99141145/Documents/develop/Project-john/Generated_Code" -I"C:/Users/99141145/Documents/develop/Project-john/Static_Code/PDD" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


