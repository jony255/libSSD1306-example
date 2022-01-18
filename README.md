# libSSD1306-example

# Description

This repository is meant to provide a working example that uses `libSSD1306`
alongside `libopencm3`. This repository assumes the device being used is the
bluepill. The bluepill is a small dev board based on the `stm32f103c8t6`
microcontroller that also comes with an additional LED on `PC13`.

# Instructions

```bash
git clone --recurse-submodules https://github.com/maybe-one-day-ubermensch/libSSD1306-example.git your-project
cd your-project
make build-libs # builds libSSD1306 and libopencm3
make # builds the example program for the bluepill (stm32f103c8t6)
```

# Using this example project with another mcu

In order to port this to another mcu, you need to change a couple of things.

## `make` variables to change

1. `DEVICE` - used by `libopencm3` to generate the linker script and defines the macros
           needed to compile your application with `libopencm3`.

2. `OPENCM3_TARGETS` - used to select which of the various `libopencm3` targets to compile

3. `OOCD_INTERFACE` - interface used by `openocd` to flash the mcu

4. `OOCD_TARGET` - target `openocd` will flash the program to

## Areas of source code to change

Some of the source code is specific to the bluepill. For example, any code that
configures/uses the additional LED is wrapped around a preprocessor conditional
block that checks if the device used is the bluepill. Feel free to erase all
code that is wrapped with the following:

```c
#ifdef STM32F103C8T6
...
#endif
```

Additionally, you will need to change the following functions:

1. `mark_as_cmd` - change to your desired GPIO port/pin
2. `mark_as_data` - change to your desired GPIO port/pin
3. `send_cmd` - change to your desired SPI peripheral, some mcus come with more than one.
4. `write_data` - change to your desired SPI peripheral, some mcus come with more than one.
5. `setup_libopencm3` - change to configure the necessary GPIO, SPI, and clocks for your microcontroller
