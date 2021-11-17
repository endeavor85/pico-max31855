#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "max31855.h"

// #define SPI_PORT spi0
// #define SPI_SCK 18
// #define SPI_RX  16
// #define SPI_CS  17

#define SPI_PORT spi1
#define SPI_SCK 14
#define SPI_RX  12
#define SPI_CS  13

int main(int argc, char **argv) {
    // turn on board LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    stdio_init_all();

    // delay before printf (otherwise it likely won't print)
    sleep_ms(500);

    MAX31855 max31855(SPI_PORT, SPI_SCK, SPI_RX, SPI_CS);

    printf("--- MAX31855 Example ---\n");

    double temp_i = 0;
    double temp_c = 0;
    double temp_f = 0;
    uint8_t err = 0x00;

    while(true) {
        temp_i = max31855.readInternal();
        temp_c = max31855.readCelsius();
        temp_f = max31855.readFahrenheit();
        err = max31855.readError();

        printf("Int: %6.2f, Cels: %6.2f, Fahr: %6.2f, Err: 0x%02x\n", temp_i, temp_c, temp_f, err);

        sleep_ms(1000);
    }
}
