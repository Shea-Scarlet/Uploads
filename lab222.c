#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO 8
#define I2C_MASTER_SDA_IO 10
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define SHTC3_I2C_ADDRESS 0x70

void initialize_i2c(){
	i2c_config_t conf = {
	.mode = I2C_MODE_MASTER,
	.sda_io_num = I2C_MASTER_SDA_IO,
	.sda_pullup_en = GPIO_PULLUP_ENABLE,
	.scl_io_num = I2C_MASTER_SCL_IO,
	.scl_pullup_en = GPIO_PULLUP_ENABLE,
	.master.clk_speed = I2C_MASTER_FREQ_HZ,
	};
	i2c_param_config(I2C_MASTER_NUM, &conf);
	i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

uint16_t read_sensor_data(float* raw_temp, float*raw_humi){

	uint8_t TEMP[2] = {0x7C, 0xA2};
	uint8_t data[6];

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SHTC3_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write(cmd, TEMP, 2, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SHTC3_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
	i2c_master_read(cmd, data, sizeof(data) - 1, I2C_MASTER_ACK);
	i2c_master_read_byte(cmd, &data[sizeof(data) - 1], I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	uint16_t temp = (data[0] << 8) | data[1];
	uint16_t humi = (data[3] << 8) | data[4];
	*raw_temp = 175 * ((float)temp / 65535.0) - 45;
	*raw_humi = 100.0 * ((float)humi / 65535.0);
	return true;
}

void app_main(void){
	float raw_temp, raw_humi;
	initialize_i2c();
	read_sensor_data(&raw_temp, &raw_humi);
	while(1) {
		float temp_fahrenheit = (raw_temp * 9/5) + 32;
		printf("Temperature is %.0f Celsius or %.0f Fahrenheit with a humidity of %0.f%%\n", raw_temp, temp_fahrenheit, raw_humi);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
