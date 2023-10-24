#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// I2C configurations
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           8        // GPIO pin for SCL
#define I2C_MASTER_SDA_IO           10        // GPIO pin for SDA
#define I2C_MASTER_FREQ_HZ          100000    // I2C master clock frequency

// SHTC3 I2C Address
#define SHTC3_I2C_ADDR              0x70

// Function to initialize I2C
static esp_err_t i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    return i2c_param_config(I2C_MASTER_NUM, &conf);
}

// Function to read temperature and humidity from SHTC3
static esp_err_t shtc3_read_data(float *temperature, float *humidity) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Send start condition
    i2c_master_start(cmd);
    
    // Write the SHTC3's I2C address with a write bit
    i2c_master_write_byte(cmd, (SHTC3_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    
    // Request temperature measurement
    i2c_master_write_byte(cmd, 0x35, true);
    
    // Delay for measurement
    vTaskDelay(pdMS_TO_TICKS(15));
    
    // Read 6 bytes for temperature and humidity data
    uint8_t data[6];
    i2c_master_read(cmd, data, 6, I2C_MASTER_LAST_NACK);
    
    // Stop condition
    i2c_master_stop(cmd);
    
    // Execute the I2C command
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Calculate temperature (in degrees Celsius) from the data
    int temp_raw = (data[0] << 8) | data[1];
    *temperature = -45 + 175.0 * (temp_raw / 65535.0);
    
    // Calculate humidity (in percentage) from the data
    int hum_raw = (data[3] << 8) | data[4];
    *humidity = 100.0 * (hum_raw / 65535.0);
    
    return ESP_OK;
}

void app_main() {
    esp_err_t ret;
    float temperature, humidity;

    // Initialize I2C
    ret = i2c_master_init();
    if (ret != ESP_OK) {
        printf("I2C initialization failed\n");
        return;
    }

    while (1) {
        // Read temperature and humidity
        ret = shtc3_read_data(&temperature, &humidity);
        if (ret == ESP_OK) {
            printf("Temperature is %.1fC (or %.1fF) with a %.0f%% humidity\n",
                   temperature, (temperature * 9 / 5 + 32), humidity);
        } else {
            printf("Failed to read data from SHTC3\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));  // Delay for 2 seconds
    }
}
