/*!
 * @file Adafruit_MAX31855.h
 *
 * This is the documentation for Adafruit's MAX31855 thermocouple breakout
 * driver for the Arduino platform.  It is designed specifically to work with
 * the Adafruit MAX31855 breakout: https://www.adafruit.com/products/269
 *
 * These sensors use SPI to communicate, 3 pins are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * Ported to Raspberry Pi Pico by endeavor85
 *
 * BSD license, all text above must be included in any redistribution.
 *
 */

#ifndef MAX31855_H
#define MAX31855_H

#include "hardware/spi.h"
#include "hardware/gpio.h"

struct MAX31855_Config {

  spi_inst_t* spi_hw_inst = PICO_DEFAULT_SPI_INSTANCE; // hardware SPI instance to use (spi0 or spi1)

  uint spi_baud = 5 * 1000000;                         // SPI baud rate

  uint spi_sck_gpio = PICO_DEFAULT_SPI_SCK_PIN;        // GPIO for SPI SCK
  uint spi_tx_gpio = PICO_DEFAULT_SPI_TX_PIN;          // GPIO for SPI TX
  uint spi_rx_gpio = PICO_DEFAULT_SPI_RX_PIN;          // GPIO for SPI RX
  uint spi_cs_gpio = PICO_DEFAULT_SPI_CSN_PIN;         // GPIO for SPI CS
};

class MAX31855 {
public:
  MAX31855(spi_inst_t* _spi_port, const uint spi_sck, const uint spi_rx, const uint8_t _spi_cs);

  double readInternal(void);
  double readCelsius(void);
  double readFahrenheit(void);
  uint8_t readError();

private:
  spi_inst_t* spi_port;
  uint8_t spi_cs;

  uint32_t spiread32(void);

  static inline void cs_select(uint pin_cs) {
    asm volatile("nop \n nop \n nop");
    gpio_put(pin_cs, 0);  // Active low
    asm volatile("nop \n nop \n nop");
  }

  static inline void cs_deselect(uint pin_cs) {
    asm volatile("nop \n nop \n nop");
    gpio_put(pin_cs, 1);
    asm volatile("nop \n nop \n nop");
  }
};

#endif
