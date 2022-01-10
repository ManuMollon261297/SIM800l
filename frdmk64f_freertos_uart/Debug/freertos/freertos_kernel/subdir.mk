################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freertos/freertos_kernel/croutine.c \
../freertos/freertos_kernel/event_groups.c \
../freertos/freertos_kernel/list.c \
../freertos/freertos_kernel/queue.c \
../freertos/freertos_kernel/stream_buffer.c \
../freertos/freertos_kernel/tasks.c \
../freertos/freertos_kernel/timers.c 

OBJS += \
./freertos/freertos_kernel/croutine.o \
./freertos/freertos_kernel/event_groups.o \
./freertos/freertos_kernel/list.o \
./freertos/freertos_kernel/queue.o \
./freertos/freertos_kernel/stream_buffer.o \
./freertos/freertos_kernel/tasks.o \
./freertos/freertos_kernel/timers.o 

C_DEPS += \
./freertos/freertos_kernel/croutine.d \
./freertos/freertos_kernel/event_groups.d \
./freertos/freertos_kernel/list.d \
./freertos/freertos_kernel/queue.d \
./freertos/freertos_kernel/stream_buffer.d \
./freertos/freertos_kernel/tasks.d \
./freertos/freertos_kernel/timers.d 


# Each subdirectory must supply rules for building sources it contributes
freertos/freertos_kernel/%.o: ../freertos/freertos_kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSERIAL_PORT_TYPE_UART=1 -DSDK_OS_FREE_RTOS -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/board" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/source" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/drivers" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/drivers/freertos" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/utilities" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/device" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/component/uart" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/component/serial_manager" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/component/lists" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/CMSIS" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/freertos/freertos_kernel/include" -I"/Users/mmollon/Documents/MCU_workspace/frdmk64f_freertos_uart/freertos/freertos_kernel/portable/GCC/ARM_CM4F" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


