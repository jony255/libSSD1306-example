#include "ssd1306/err.h"
#include "ssd1306/font.h"
#include "ssd1306/platform.h"
#include "ssd1306/ssd1306.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef STM32F103C8T6

#define LED_RCC_PORT RCC_GPIOC
#define LED_PORT     GPIOC
#define LED_PIN      GPIO13

#endif

#define DC_PORT GPIOA
#define DC_PIN  GPIO6

/**
 * Let the SSD1306 know the following 8 bits should be interpreted as data.
 */
static void
mark_as_cmd(void)
{
    gpio_clear(DC_PORT, DC_PIN);
}

/**
 * Let the SSD1306 know the following 8 bits should be interpreted as data.
 */
static void
mark_as_data(void)
{
    gpio_set(DC_PORT, DC_PIN);
}

#ifdef STM32F103C8T6

/**
 * The anode of the LED is tied to Vcc and its cathode is connected to 'C13'.
 * In order to turn it on, clear 'C13'.
 */
static void
turn_led_on(void)
{
    gpio_clear(LED_PORT, LED_PIN);
}

/**
 * The anode of the LED is tied to Vcc and its cathode is connected to 'C13'.
 * In order to turn it off, set 'C13'.
 */
static void
turn_led_off(void)
{
    gpio_set(LED_PORT, LED_PIN);
}

static void
toggle_led(void)
{
    gpio_toggle(LED_PORT, LED_PIN);
}

#endif

static void
setup_libopencm3(void)
{
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_24MHZ]);

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_SPI1);
#ifdef STM32F103C8T6
    rcc_periph_clock_enable(LED_RCC_PORT);
#endif

    /*
     * CS   = A4
     * SCLK = A5
     * MOSI = A7
     */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO4 | GPIO5 | GPIO7);

    /*
     * D/C = A6
     *
     * Don't configure A6 as MISO since serial reads aren't possible with
     * the SSD1306. Instead, we will use it as the D/C pin.
     */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO6);

    /*
     * The ssd1306 operates in SPI mode 0.
     * The ssd1306 expects data with the MSB first.
     */
    spi_init_master(
        SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_256, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
        SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

    /* Let the hardware handle chip select. */
    spi_disable_software_slave_management(SPI1);

    /*
     * Let the stm32f1 drive chip select low once the stm32f1 (master) begins
     * communication with the ssd1306 up until SPI1 is disabled.
     */
    spi_enable_ss_output(SPI1);

    /*
     * Therefore, configure the stm32f1 to only transmit and leave the unused
     * MISO pin free for use as D/C.
     */
    spi_set_full_duplex_mode(SPI1);
    spi_set_unidirectional_mode(SPI1);

#ifdef STM32F103C8T6
    gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  LED_PIN);
    turn_led_off();
#endif
}

static void
delay(unsigned int delay)
{
    for (volatile unsigned int i = 0; i < delay; i++) {
        /* The nop might be unnecessary since i is volatile. */
        __asm__("nop");
    }
}

enum ssd1306_err
send_cmd(struct ssd1306_ctx *ctx, uint8_t cmd)
{
    spi_enable(SPI1);

    mark_as_cmd();

    spi_send(SPI1, (uint8_t)cmd);

    spi_clean_disable(SPI1);

    return SSD1306_OK;
}

enum ssd1306_err
write_data(struct ssd1306_ctx *ctx, uint8_t data)
{
    spi_enable(SPI1);

    mark_as_data();

    spi_send(SPI1, data);

    spi_clean_disable(SPI1);

    return SSD1306_OK;
}

int
main(void)
{

    setup_libopencm3();

    struct ssd1306_ctx _ctx = {
        .send_cmd = send_cmd,
        .write_data = write_data,
        .height = 64,
        .width = 128,
    };

    struct ssd1306_ctx *ctx = &_ctx;

    if (ssd1306_init_display(ctx, true) != SSD1306_OK) {
        turn_led_on();
    }

    if (ssd1306_write_str(ctx, (const uint8_t *)"Hello World!") != SSD1306_OK) {
        turn_led_on();
    }

    /*
     * In order for the program to not immediately exit, tell the mcu to
     * stop executing the program and wait for an interrupt. It's better
     * than looping indefinitely. If there are problems halting at this
     * point, then replace this line with an infinite loop. Refer to the
     * 'halt' and 'wait_halt' command in
     * https://openocd.org/doc-release/html/General-Commands.html for more
     * information.
     */
    __asm__("wfi");

    return 0;
}
