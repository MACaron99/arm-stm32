
CPU     = cortex-m0
PART    = STM32F030
F_XTAL  = 16000000UL

PROJECT = stm32-03
TARGETS = $(BINDIR)/$(PROJECT).elf \
          $(BINDIR)/$(PROJECT).bin \
          size

STM32FLASH_EXTRA = -e 30

STM32_LIBRARY_PATH    = $(EMLIB)/stm32/Libraries/STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries
STM32_DRIVERS_PATH    = $(STM32_LIBRARY_PATH)/STM32F0xx_StdPeriph_Driver
CMSIS_PATH            = $(STM32_LIBRARY_PATH)/CMSIS
STM32_PART_PATH       = $(CMSIS_PATH)/Device/ST/STM32F0xx


# Include the common make definitions
include $(EMLIB)/stm32/makedefs

#
# Common definitions
#
COMMON += -D${PART}
COMMON += -DUSE_STDPERIPH_DRIVER

ifdef F_XTAL
COMMON += -DHSE_VALUE=${F_XTAL}
else
COMMON += -DINTERNAL_CLOCK_SOURCE
endif

## System include
IPATH   = $(CMSIS_PATH)/Include               \
          $(STM32_PART_PATH)/Include          \
          $(STM32_DRIVERS_PATH)/inc           \
          src/comm                            \
          src/comm/intra_comm                 \
          src/drivers                         \
          src/drivers/indicator               \
          src/lib                             \
          src/lib/filter                      \
          src/system                          \
          src/modules                         \
          src/app                             \
          src/ui                              \
          src/modbus-slave                    \
          src/modbus-master                   \
          src/.

## System path
VPATH   = $(STM32_DRIVERS_PATH)/src           \
          src/comm                            \
          src/comm/intra_comm                 \
          src/drivers                         \
          src/drivers/indicator               \
          src/lib                             \
          src/lib/filter                      \
          src/system                          \
          src/modules                         \
          src/app                             \
          src/ui                              \
          src/modbus-slave                    \
          src/modbus-master                   \
          src/.


## SPL section
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_rcc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_adc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_gpio.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_exti.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_spi.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_tim.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_flash.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_usart.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_dma.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_syscfg.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_iwdg.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_misc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/stm32f0xx_pwr.o

## Drivers section
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/adc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/storage.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/uart.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/uart_sw.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/indicator.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/modbus.o

## Lib section
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/pi.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/crc.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/imath.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/ustdlib.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/temp_sensor.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/ustdlib.o

$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/filter_iir_simple.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/filter_iir1.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/filter_median3.o

## Comm section


## Apps


## Modules
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/aps.o

## Modbus

## System section
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sys_error.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sys_timer.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sys_param.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sys_param_ext.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sys_service.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/sys_process.o

## UI

## Main section
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/pt.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/startup.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/core.o
$(BINDIR)/$(PROJECT).elf: ${OBJDIR}/main.o

