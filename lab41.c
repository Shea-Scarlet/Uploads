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
    // Initialize your ICM-42670-P sensor here, configure it, and set up the I2C or SPI interface.
    initialize_i2c();

    while (1)
    {
        // Read sensor data and calculate inclination.
        float inclination_x = /* Calculate inclination in X direction */;
        float inclination_y = /* Calculate inclination in Y direction */;

        // Determine the inclination based on sensor data.
        const char* inclination_str = "UNKNOWN";

        if (inclination_x > 0.5) {
            inclination_str = "UP";
        } else if (inclination_x < -0.5) {
            inclination_str = "DOWN";
        }

        if (inclination_y > 0.5) {
            if (inclination_str == "UNKNOWN") {
                inclination_str = "LEFT";
            } else {
                inclination_str = "UP LEFT";
            }
        } else if (inclination_y < -0.5) {
            if (inclination_str == "UNKNOWN") {
                inclination_str = "RIGHT";
            } else {
                inclination_str = "UP RIGHT";
            }
        }

        // Print the inclination to the terminal using ESP_LOGI.
        ESP_LOGI(TAG, "Board inclination: %s", inclination_str);

        vvTaskDelay(pdMS_TO_TICKS(1000)); // Adjust the delay as needed.
    }
}
