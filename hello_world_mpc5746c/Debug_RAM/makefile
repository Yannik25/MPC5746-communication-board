################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Sources/subdir.mk
-include SDK/platform/drivers/src/pins/siul2/subdir.mk
-include SDK/platform/devices/MPC5746C/startup/subdir.mk
-include SDK/platform/devices/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include Project_Settings/Linker_Files/subdir.mk
-include Generated_Code/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
hello_world_mpc5746c.siz \


# All Target
all: hello_world_mpc5746c.elf secondary-outputs

# Tool invocations
hello_world_mpc5746c.elf: $(OBJS) C:\NXP\S32DS_Power_v2.1\S32DS\software\S32_SDK_S32PA_RTM_3.0.0\platform\devices\MPC5746C\linker\gcc\MPC5746C_Z4\MPC5746C_ram.ld $(OSPT) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	powerpc-eabivle-gcc -o "hello_world_mpc5746c.elf" "@hello_world_mpc5746c.args"  $(USER_OBJS) $(LIBS) $(OSPT)
	@echo 'Finished building target: $@'
	@echo ' '

hello_world_mpc5746c.siz: hello_world_mpc5746c.elf
	@echo 'Invoking: Standard S32DS Print Size'
	powerpc-eabivle-size --format=berkeley hello_world_mpc5746c.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
