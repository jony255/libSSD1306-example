# libSSD1306
SSD1306_DIR 	  = libSSD1306
SSD1306_BUILD_DIR = $(SSD1306_DIR)/build
SSD1306_INCLUDE   = $(SSD1306_DIR)/include
SSD1306_LIB	      = ssd1306

# libopencm3
OPENCM3_DIR 	= libopencm3
DEVICE 			= stm32f103c8t6
OPENCM3_TARGETS = stm32/f1

# Add libSSD1306 to flags recognized by libopencm3's build system.
INCLUDES += -I$(SSD1306_INCLUDE)
LDFLAGS  += -L$(SSD1306_BUILD_DIR)
LDLIBS   += -l$(SSD1306_LIB)

# our example project
PROJECT = libssd1306-test
BUILD_DIR = build

CFILES = src/main.c
CFLAGS += -ggdb3 -ffunction-sections -fdata-sections -Wno-missing-prototypes

# openocd
OOCD_INTERFACE = stlink
OOCD_TARGET = stm32f1x
OOCD_GDB_LOG = /dev/null
OOCD_GDB_REMOTE_TIMEOUT= unlimited

default: elf objcopy

elf: $(BUILD_DIR)/$(PROJECT).elf
objcopy: $(BUILD_DIR)/$(PROJECT).bin

build-libSSD1306:
	# defaults to a debug build
	# add --build-type=release to the options for a release build
	meson\
		--cross-file machine-files/arm-none-eabi-gcc.ini\
		--cross-file machine-files/embedded-common-flags.ini\
		--cross-file machine-files/bluepill.ini\
		$(SSD1306_BUILD_DIR)\
		$(SSD1306_DIR)

	ninja -C $(SSD1306_BUILD_DIR)

build-libopencm3:
	$(MAKE) -C $(OPENCM3_DIR) TARGETS=$(OPENCM3_TARGETS)

build-libs: build-libSSD1306 build-libopencm3

gdb: elf
	arm-none-eabi-gdb $(BUILD_DIR)/$(PROJECT).elf \
		-ex 'set remotetimeout $(OOCD_GDB_REMOTE_TIMEOUT)'\
		-ex 'target extended-remote |\
			openocd -f interface/$(OOCD_INTERFACE).cfg\
					-f target/$(OOCD_TARGET).cfg\
					-c "gdb_port pipe; log_output $(OOCD_GDB_LOG)"'\
		-ex 'monitor reset halt'\
		-ex 'load'\
		-ex 'b main'\
		-ex 'continue'


include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

.PHONY: default gdb
