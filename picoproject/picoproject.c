#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include <stdio.h>

#define ANALOG_GPIO_PIN 26
#define I2C_ADDR 0x5
#define GPIO_SDA0 0
#define GPIO_SCK0 1

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

volatile ushort value;
void i2c0_irq_handler()
{
    uint32_t status = i2c0->hw->intr_stat;
    uint32_t data = i2c0->hw->data_cmd & I2C_IC_DATA_CMD_DAT_BITS;

    if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {

        printf("Requested register: %u\r\n", data);
        i2c0->hw->clr_rd_req;

        value = adc_read();
        i2c_write_raw_blocking(i2c0, (void*)&value, sizeof(value));
        printf("Sent value: %u\r\n", value);
    }
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(ANALOG_GPIO_PIN);
    adc_select_input(0);

    i2c_init(i2c0, 100000);
    i2c_set_slave_mode(i2c0, true, I2C_ADDR);
    gpio_set_function(GPIO_SDA0, GPIO_FUNC_I2C);
    gpio_set_function(GPIO_SCK0, GPIO_FUNC_I2C);

    i2c0->hw->intr_mask = I2C_IC_INTR_MASK_M_RD_REQ_BITS;
    irq_set_exclusive_handler(I2C0_IRQ, i2c0_irq_handler);
    irq_set_enabled(I2C0_IRQ, true);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_put(LED_PIN, 1);
    sleep_ms(1000);
    gpio_put(LED_PIN, 0);
    printf("Monitoring initialized\r\n");

    while (true)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        value = adc_read();
        printf("Monitor: %u\r\n", value);
        gpio_put(LED_PIN, 0);
        sleep_ms(900);
    }

    return 0;
}