#include <stdio.h>
#include "driver/i2c.h"
#include "shtc3.h" // Assuming you have a library for SHTC3

#define I2C_MASTER_SCL_IO 0x10 /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 0x8 /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1 /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master do not need buffer */

void app_main() {
    // setup i2c controller
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 1000000
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    shtc3_t shtc3;
    shtc3.i2c_addr = SHTC3_I2C_ADDRESS; // replace with your device's address
    shtc3.i2c_port = I2C_MASTER_NUM;

    while (1) {
        float temp_c, temp_f, humidity;

        // Read temperature and humidity
        if (shtc3_read(&shtc3, &temp_c, &humidity) != 0) {
            printf("Failed to read from SHTC3\n");
            return -1;
        }

        // Convert temperature to Fahrenheit
        temp_f = temp_c * 9.0 / 5.0 + 32.0;

        // Print temperature and humidity
        printf("Temperature is %.0fC (or %.0fF) with a %.0f%% humidity\n", temp_c, temp_f, humidity);

        // Wait for 2 seconds
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
