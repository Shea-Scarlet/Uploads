#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// I2C configurations
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           8        // GPIO pin for SCL
#define I2C_MASTER_SDA_IO           10        // GPIO pin for SDA
#define I2C_MASTER_FREQ_HZ          100000    // I2C master clock frequency
#define TRIG_PIN 		    1
#define ECHO_PIN 		    0

// Speed of sound in air at 20 degrees Celsius
#define SOUND_SPEED_AT_20C 343.2

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

/*/
void init_ultrasonic_sensor() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TRIG_PIN),
        .mode = GPIO_MODE_OUTPUT,
	.intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    gpio_config_t echo = {
        .pin_bit_mask = (1ULL << ECHO_PIN),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&echo);
}
/*/
void init_ultrasonic_sensor() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TRIG_PIN) | (1ULL << ECHO_PIN),
        .mode = GPIO_MODE_OUTPUT,
	.intr_type = GPIO_INTR_DISABLE,
    	.pull_down_en = GPIO_PULLDOWN_DISABLE,
    	.pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&io_conf);
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

float measure_distance() {
    // Generate a 10us pulse to trigger the ultrasonic sensor
    gpio_set_level(TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_PIN, 0);
    // Measure the time taken for the ultrasonic wave to return
    while (gpio_get_level(ECHO_PIN) == 0) {}
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN) == 1) {}
    int64_t end = esp_timer_get_time();

    // Calculate distance using the speed of sound and the time taken
    float distance = (end - start) * SOUND_SPEED_AT_20C / 2 / 1000000.0; // in cm
    return distance;
}

void app_main (void) {
    float temperature, humidity, distance;
    initialize_i2c();
    init_ultrasonic_sensor();
    while(1) {
        read_sensor_data(&temperature, &humidity);
        distance = measure_distance();
        printf("Distance: %.0f cm at %.0fC\n", distance, temperature);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
