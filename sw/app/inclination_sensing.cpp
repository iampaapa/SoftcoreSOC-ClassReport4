#include "chu_init.h"
#include "gpio_cores.h"
#include "spi_core.h"
#include "math.h"

// ADXL362 register definitions
const uint8_t ADXL_CMD_WRITE = 0x0A;
const uint8_t ADXL_CMD_READ  = 0x0B;
const uint8_t ADXL_REG_XDATA8 = 0x08; // 8-bit X acceleration
const uint8_t ADXL_REG_YDATA8 = 0x09; // 8-bit Y acceleration
const uint8_t ADXL_REG_ZDATA8 = 0x0A; // 8-bit Z acceleration
const uint8_t ADXL_REG_POWER_CTL = 0x2D; 

void adxl_write(SpiCore *spi, uint8_t reg, uint8_t data) {
    // SPI Mode 0, MSB first
    spi->assert_ss(0);
    spi->transfer(ADXL_CMD_WRITE);
    spi->transfer(reg);
    spi->transfer(data);
    spi->deassert_ss(0);
}

// read from ADXL362 Register
uint8_t adxl_read(SpiCore *spi, uint8_t reg) {
    uint8_t data;
    spi->assert_ss(0);
    spi->transfer(ADXL_CMD_READ);
    spi->transfer(reg);
    data = spi->transfer(0x00);
    spi->deassert_ss(0);
    return data;
}

// main sensing application
int main() {
    // 1. instantiate cores
    GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
    SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));

    // 2. initialize SPI
    spi.set_freq(400000); // set SPI clock to 400kHz
    spi.set_mode(0, 0);

    // 3. initialize ADXL362
    adxl_write(&spi, ADXL_REG_POWER_CTL, 0x02);
    sleep_ms(100);

    // vars for acceleration data
    int8_t x, y;
    const int8_t THRESHOLD = 30; 

    while (1) {
        // 4. get the data
        x = (int8_t)adxl_read(&spi, ADXL_REG_XDATA8);
        y = (int8_t)adxl_read(&spi, ADXL_REG_YDATA8);

        int led_pattern = 0;

        if (abs(x) > abs(y) && abs(x) > THRESHOLD) {
            if (x > 0) {
                // board tilted left -> 270 deg
                led_pattern = 0b1000; // LED[3]
            } else {
                // board tilted right -> 90 deg
                led_pattern = 0b0010; // LED[1]
            }
        } 
        else if (abs(y) > abs(x) && abs(y) > THRESHOLD) {
            if (y > 0) {
                // board tilted top-down -> 180 deg
                led_pattern = 0b0100; // LED[2]
            } else {
                // board upright -> 0 deg
                led_pattern = 0b0001; // LED[0]
            }
        } 
        else {
            led_pattern = 0b1111; // all the LEDs should be on if its flat
        }

        led.write(led_pattern);
        sleep_ms(200); 
    }

    return 0;
}