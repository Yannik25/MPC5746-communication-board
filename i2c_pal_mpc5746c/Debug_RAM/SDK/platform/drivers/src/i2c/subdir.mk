################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/NXP/S32DS_Power_v2.1/S32DS/software/S32_SDK_S32PA_RTM_3.0.0/platform/drivers/src/i2c/i2c_driver.c \
C:/NXP/S32DS_Power_v2.1/S32DS/software/S32_SDK_S32PA_RTM_3.0.0/platform/drivers/src/i2c/i2c_irq.c 

OBJS += \
./SDK/platform/drivers/src/i2c/i2c_driver.o \
./SDK/platform/drivers/src/i2c/i2c_irq.o 

C_DEPS += \
./SDK/platform/drivers/src/i2c/i2c_driver.d \
./SDK/platform/drivers/src/i2c/i2c_irq.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/drivers/src/i2c/i2c_driver.o: C:/NXP/S32DS_Power_v2.1/S32DS/software/S32_SDK_S32PA_RTM_3.0.0/platform/drivers/src/i2c/i2c_driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@SDK/platform/drivers/src/i2c/i2c_driver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SDK/platform/drivers/src/i2c/i2c_irq.o: C:/NXP/S32DS_Power_v2.1/S32DS/software/S32_SDK_S32PA_RTM_3.0.0/platform/drivers/src/i2c/i2c_irq.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@SDK/platform/drivers/src/i2c/i2c_irq.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


