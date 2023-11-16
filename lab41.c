#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "math.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

// I2C configurations
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           8        // GPIO pin for SCL
#define I2C_MASTER_SDA_IO           10        // GPIO pin for SDA
#define I2C_MASTER_FREQ_HZ          100000    // I2C master clock frequency

#define PWR_CONFIG 0x1F
#define PWR_SETTINGS 12

#define ICM42670_P_I2C_ADDR 0x68

#define ACCEL_DATA_X1 0x2D
#define ACCEL_DATA_X0 0x2E
#define ACCEL_DATA_Y1 0x2F
#define ACCEL_DATA_Y0 0x30

#define ACCEL_CONFIG 0x14
#define ACCEL_SETTINGS 8
#define ACCEL_THRESH 1500

// SHTC3 I2C Address
//#define SHTC3_I2C_ADDR              0x70

// ICM I2C Address
//#define REGISTER_TO_READ_WRITE 0x01

//static const char *TAG = "ICM42670_P";

void initialize_i2c () {
// Function to initialize I2C
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

uint8_t read_byte_i2c (uint8_t read) {
    // Read a byte from the ICM-42670-P register
    uint8_t data;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM42670_P_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, read, true);
	
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM42670_P_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
	
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 2000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return data;
}

/*/
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C read error: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Read data: 0x%02X", data);
    }
/*/

void write_byte_i2c (uint8_t read, uint8_t data_to_write) {

    // Write a byte to the ICM-42670-P register
    //uint8_t data_to_write = 0xAB; // Replace with the data you want to write
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM42670_P_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, read, true);
    i2c_master_write_byte(cmd, data_to_write, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 2000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    /*/
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write error: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Write data: 0x%02X", data_to_write);
    }
    /*/

    // Deinitialize the I2C bus when done
    //i2c_driver_delete(I2C_NUM_0);
	
}

void app_main(void)
{
    // Initialize your ICM-42670-P sensor here and configure it.
    initialize_i2c();

    write_byte_i2c(PWR_CONFIG, PWR_SETTINGS);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    write_byte_i2c(ACCEL_CONFIG, ACCEL_SETTINGS);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    while(1)
    {
	
    	int16_t acceleration_x = (read_byte_i2c(ACCEL_DATA_X1) << 8) | read_byte_i2c(ACCEL_DATA_X0);
    	int16_t acceleration_y = (read_byte_i2c(ACCEL_DATA_Y1) << 8) | read_byte_i2c(ACCEL_DATA_Y0);

        // Read accelerometer data from the sensor.
        // int16_t accelerometer_data[3];
        // Read accelerometer data from the sensor using your driver.
        // You might need to adapt this part to your specific driver.
	/*/
        if (read_accelerometer_data(accelerometer_data) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read accelerometer data");
            continue;
        }
	/*/
	    
        // Calculate inclinations based on accelerometer data.
        //float acceleration_x = accelerometer_data[0] / 16384.0; // Replace with your sensor's specific scale factor.
        //float acceleration_y = accelerometer_data[1] / 16384.0; // Replace with your sensor's specific scale factor.

        // Determine the inclination based on sensor data.
        //char* inclination_x_str = "UNKNOWN";
        //char* inclination_y_str = "UNKNOWN";

	    /*/
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
	    /*/

        // Determine the inclination based on sensor data and print directly.
        if (acceleration_x > 1500) {
        	if (acceleration_y > 1500) {
                    printf("Inclination: UP RIGHT");
                } else if (acceleration_y < -1500) {
                    printf("Inclination: DOWN RIGHT");
                } else {
                    printf("Inclination: RIGHT");
                }
        } else if (acceleration_x < -1500) {
                if (acceleration_y > 1500) {
                    printf("Inclination: UP LEFT");
                } else if (acceleration_y < -1500) {
                    printf("Inclination: DOWN LEFT");
                } else {
                    printf("Inclination: LEFT");
                }
	} else if (acceleration_y > 1500) {
                printf("Inclination: UP");
        } else if (acceleration_y < -1500) {
                printf("Inclination: DOWN");
        } else {
                printf("Inclination: UNKNOWN");
        }

	vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
