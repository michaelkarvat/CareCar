# CareCar firmware - command line build
#
# Mirrors the STM32CubeIDE managed build so the project can also be built from
# a terminal or from CI with only the GNU Arm Embedded toolchain installed.
#
#   make            build build/carecar.elf, .bin and .hex
#   make flash      flash the board with st-flash
#   make clean      remove build output

TARGET      := carecar
BUILD_DIR   := build

PREFIX      := arm-none-eabi-
CC          := $(PREFIX)gcc
OBJCOPY     := $(PREFIX)objcopy
SIZE        := $(PREFIX)size

LINKER_SCRIPT := STM32F303RETX_FLASH.ld

C_SOURCES   := $(wildcard Src/*.c)
ASM_SOURCES := $(wildcard Startup/*.s)
OBJECTS     := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o))

# Cortex-M4 with the single precision FPU, as fitted on the STM32F303RE.
CPU_FLAGS   := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

DEFINES     := -DSTM32 -DSTM32F3 -DSTM32F303RETx

# -ffunction-sections/-fdata-sections plus --gc-sections at link time drop
# every unreferenced symbol, which matters on a 512 KB part with no HAL.
CFLAGS      := $(CPU_FLAGS) $(DEFINES) -std=gnu11 -Og -g3 \
               -IInc -Wall -Wextra \
               -ffunction-sections -fdata-sections \
               --specs=nano.specs -MMD -MP

ASFLAGS     := $(CPU_FLAGS) -g3

# nosys.specs supplies stub syscalls; the firmware never uses stdio streams.
LDFLAGS     := $(CPU_FLAGS) -T$(LINKER_SCRIPT) \
               --specs=nano.specs --specs=nosys.specs -static \
               -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
               -Wl,--start-group -lc -lm -Wl,--end-group

.PHONY: all clean flash

all: $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $< 0x8000000

clean:
	rm -rf $(BUILD_DIR)

-include $(OBJECTS:.o=.d)
