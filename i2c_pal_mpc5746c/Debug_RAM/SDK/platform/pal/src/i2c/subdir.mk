################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/NXP/S32DS_Power_v2.1/S32DS/software/S32_SDK_S32PA_RTM_3.0.0/platform/pal/src/i2c/i2c_pal.c 

OBJS += \
./SDK/platform/pal/src/i2c/i2c_pal.o 

C_DEPS += \
./SDK/platform/pal/src/i2c/i2c_pal.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/pal/src/i2c/i2c_pal.o: C:/NXP/S32DS_Power_v2.1/S32DS/software/S32_SDK_S32PA_RTM_3.0.0/platform/pal/src/i2c/i2c_pal.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@SDK/platform/pal/src/i2c/i2c_pal.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

