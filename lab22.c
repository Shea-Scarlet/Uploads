#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SHTC3_I2C_ADDRESS 0x70
#define I2C_MASTER_SCL_IO 0x10
#define I2C_MASTER_SDA_IO 0x8
#define I2C_MASTER_NUM I2C_NUM_0

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

    while (1) {
        uint8_t data[6];
        i2c_cmd_handle_t cmd;

        // Send command to read temperature and humidity
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (SHTC3_I2C_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_EN);
        i2c_master_write_byte(cmd, 0xE0, ACK_CHECK_EN); // replace with command to read temperature and humidity
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);

        vTaskDelay(20 / portTICK_RATE_MS); // wait for measurement

        // Read temperature and humidity data
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (SHTC3_I2C_ADDRESS << 1) | READ_BIT, ACK_CHECK_EN);
        i2c_master_read(cmd, data, sizeof(data), LAST_NACK_VAL);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);

        // Convert and print temperature and humidity
        float temp_c = ((data[0] << 8) | data[1]) * 175.0 / 65536.0 - 45.0;
        float temp_f = temp_c * 9.0 / 5.0 + 32.0;
        float humidity = ((data[3] << 8) | data[4]) * 100.0 / 65536.0;
        
        printf("Temperature is %.0fC (or %.0fF) with a %.0f%% humidity\n", temp_c, temp_f, humidity);

        vTaskDelay(2000 / portTICK_RATE_MS); // wait for 2 seconds
    }
}
