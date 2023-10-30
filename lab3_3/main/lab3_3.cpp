#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DFRobot_LCD.h"

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

// Function to read temperature and humidity from SHTC3
uint16_t read_sensor_data(float* temperature, float* humidity){

    // Read 6 bytes for temperature and humidity data
    uint8_t data[6];
    uint8_t TEMP[2] = {0x7C, 0xA2};
	
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Send start condition
    i2c_master_start(cmd);
    
    // Write the SHTC3's I2C address with a write bit
    i2c_master_write_byte(cmd, (SHTC3_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    
    // Request temperature measurement
    i2c_master_write(cmd, TEMP, 2, true);

    // Stop condition
    i2c_master_stop(cmd);

    // Execute the I2C command
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);

    //delete
    i2c_cmd_link_delete(cmd);

	
    cmd = i2c_cmd_link_create();
    
    // Send start condition
    i2c_master_start(cmd);
    
    // Write the SHTC3's I2C address with a write bit
    i2c_master_write_byte(cmd, (SHTC3_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    
    // Read temperature measurement
    i2c_master_read(cmd, data, sizeof(data), I2C_MASTER_ACK);
    //i2c_master_read_byte(cmd, &data[sizeof(data) - 1], I2C_MASTER_NACK);

    // Stop condition
    i2c_master_stop(cmd);

    // Execute the I2C command
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);

    //delete
    i2c_cmd_link_delete(cmd);

    
    // Calculate temperature (in degrees Celsius) from the data
    uint16_t temp = (data[0] << 8) | data[1];
    *temperature = 175.0 * ((float)temp / 65535.0) -45;
    
    // Calculate humidity (in percentage) from the data
    uint16_t humi = (data[3] << 8) | data[4];
    *humidity = 100.0 * ((float)humi / 65535.0);
    
    return true;
}

void app_main(void) {
    float temperature, humidity;

    DFRobot_LCD lcd(16, 2);


    // Initialize I2C
    initialize_i2c();
    read_sensor_data(&temperature, &humidity);

    while (1) {
        // Read temperature and humidity
	lcd.init();
	lcd.setRGB(0, 255, 0);
	read_sensor_data(&temperature, &humidity);
        lcd.printf("Temp: %.0fC", temperature);
	lcd.cursor(0, 1);
        lcd.printf("Hum: %0.f%%", humidity);
        vTaskDelay(pdMS_TO_TICKS(2000));  // Delay for 2 seconds
    }
}
