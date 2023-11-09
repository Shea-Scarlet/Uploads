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
void initialize_i2c () {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
	.sda_pullup_en = GPIO_PULLUP_ENABLE,
	.scl_pullup_en = GPIO_PULLUP_ENABLE,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}


static const char* TAG = "InclinationSensor";

void app_main(void)
{
    // Initialize your ICM-42670-P sensor here and configure it.
	initialize_i2c();
    // Replace with actual initialization and configuration code.

    while (1)
    {
        // Read accelerometer data from the sensor.
        int16_t accelerometer_data[3];
        // Read accelerometer data from the sensor using your driver.
        // You might need to adapt this part to your specific driver.
        if (read_accelerometer_data(accelerometer_data) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read accelerometer data");
            continue;
        }

        // Calculate inclinations based on accelerometer data.
        float acceleration_x = accelerometer_data[0] / 16384.0; // Replace with your sensor's specific scale factor.
        float acceleration_y = accelerometer_data[1] / 16384.0; // Replace with your sensor's specific scale factor.

        // Determine the inclination based on sensor data.
        const char* inclination_x_str = "UNKNOWN";
        const char* inclination_y_str = "UNKNOWN";

        if (acceleration_x > 0.5) {
            inclination_x_str = "RIGHT";
        } else if (acceleration_x < -0.5) {
            inclination_x_str = "LEFT";
        }

        if (acceleration_y > 0.5) {
            inclination_y_str = "UP";
        } else if (acceleration_y < -0.5) {
            inclination_y_str = "DOWN";
        }

        // Print the inclinations to the terminal using ESP_LOGI.
        if (strcmp(inclination_x_str, "UNKNOWN") == 0 && strcmp(inclination_y_str, "UNKNOWN") == 0) {
            ESP_LOGI(TAG, "Inclination: UNKNOWN");
        } else if (strcmp(inclination_x_str, "UNKNOWN") == 0) {
            ESP_LOGI(TAG, "Inclination: %s", inclination_y_str);
        } else if (strcmp(inclination_y_str, "UNKNOWN") == 0) {
            ESP_LOGI(TAG, "Inclination: %s", inclination_x_str);
        } else {
            ESP_LOGI(TAG, "Combined Inclination: %s %s", inclination_y_str, inclination_x_str);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Adjust the delay as needed.

        vvTaskDelay(pdMS_TO_TICKS(1000)); // Adjust the delay as needed.
    }
}
