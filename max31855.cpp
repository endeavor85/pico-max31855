/*!
 * @file MAX31855.cpp
 *
 * @mainpage Adafruit MAX31855 Thermocouple Breakout Driver (ported to rpi pico)
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's MAX31855 thermocouple breakout
 * driver for the Arduino platform.  It is designed specifically to work with
 * the Adafruit MAX31855 breakout: https://www.adafruit.com/products/269
 *
 * This breakout uses SPI to communicate, 3 pins are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a
 * href="https://github.com/adafruit/Adafruit_BusIO"> Adafruit_BusIO</a> being
 * present on your system. Please make sure you have installed the latest
 * version before using this library.
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * Ported by endeavor85
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution.
 *
 */

#include "max31855.h"

#include <stdlib.h>

#include "pico/binary_info.h"

MAX31855::MAX31855(spi_inst_t* _spi_port, const uint spi_sck, const uint spi_rx, const uint8_t _spi_cs) :
  spi_port(_spi_port),
  spi_cs(_spi_cs)
{
  gpio_set_function(spi_sck, GPIO_FUNC_SPI);
  gpio_set_function(spi_rx, GPIO_FUNC_SPI);

  // Chip select is active-low, so we'll initialise it to a driven-high state
  gpio_init(spi_cs);
  gpio_set_dir(spi_cs, GPIO_OUT);
  gpio_put(spi_cs, 1);
  // Make the CS pin available to picotool
  bi_decl(bi_1pin_with_name(spi_cs, "SPI CS"));

  // 5MHz SPI baud rate
  spi_init(spi_port, 5000000);
  
  cs_deselect(spi_cs);
}

/**************************************************************************/
/*!
    @brief  Read the internal temperature.

    @return The internal temperature in degrees Celsius.
*/
/**************************************************************************/
double MAX31855::readInternal(void) {
  uint32_t v;

  v = spiread32();

  // ignore bottom 4 bits - they're just thermocouple data
  v >>= 4;

  // pull the bottom 11 bits off
  float internal = v & 0x7FF;
  // check sign bit!
  if (v & 0x800) {
    // Convert to negative value by extending sign and casting to signed type.
    int16_t tmp = 0xF800 | (v & 0x7FF);
    internal = tmp;
  }
  internal *= 0.0625; // LSB = 0.0625 degrees
  // Serial.print("\tInternal Temp: "); Serial.println(internal);
  return internal;
}

/**************************************************************************/
/*!
    @brief  Read the thermocouple temperature.

    @return The thermocouple temperature in degrees Celsius.
*/
/**************************************************************************/
double MAX31855::readCelsius(void) {

  int32_t v;

  v = spiread32();

  // Serial.print("0x"); Serial.println(v, HEX);

  /*
  float internal = (v >> 4) & 0x7FF;
  internal *= 0.0625;
  if ((v >> 4) & 0x800)
    internal *= -1;
  Serial.print("\tInternal Temp: "); Serial.println(internal);
  */

  if (v & 0x7) {
    // uh oh, a serious problem!
    return (0.0F/0.0F);
  }

  if (v & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    v = 0xFFFFC000 | ((v >> 18) & 0x00003FFF);
  } else {
    // Positive value, just drop the lower 18 bits.
    v >>= 18;
  }
  // Serial.println(v, HEX);

  double centigrade = v;

  // LSB = 0.25 degrees C
  centigrade *= 0.25;
  return centigrade;
}

/**************************************************************************/
/*!
    @brief  Read the error state.

    @return The error state.
*/
/**************************************************************************/
uint8_t MAX31855::readError() { return spiread32() & 0x7; }

/**************************************************************************/
/*!
    @brief  Read the thermocouple temperature.

    @return The thermocouple temperature in degrees Fahrenheit.
*/
/**************************************************************************/
double MAX31855::readFahrenheit(void) {
  float f = readCelsius();
  f *= 9.0;
  f /= 5.0;
  f += 32;
  return f;
}

/**************************************************************************/
/*!
    @brief  Read 4 bytes (32 bits) from breakout over SPI.

    @return The raw 32 bit value read.
*/
/**************************************************************************/
uint32_t MAX31855::spiread32(void) {
  uint32_t d = 0;
  uint8_t buf[4];

  cs_select(spi_cs);
  spi_read_blocking(spi_port, 0, buf, 4);
  cs_deselect(spi_cs);

  d = buf[0];
  d <<= 8;
  d |= buf[1];
  d <<= 8;
  d |= buf[2];
  d <<= 8;
  d |= buf[3];

  return d;
}
