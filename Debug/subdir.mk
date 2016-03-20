################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Server.c \
../apiLoader.c \
../base64.c \
../main.c \
../requestHandler.c \
../responseWriter.c \
../utils.c 

OBJS += \
./Server.o \
./apiLoader.o \
./base64.o \
./main.o \
./requestHandler.o \
./responseWriter.o \
./utils.o 

C_DEPS += \
./Server.d \
./apiLoader.d \
./base64.d \
./main.d \
./requestHandler.d \
./responseWriter.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -pthread -lm -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


