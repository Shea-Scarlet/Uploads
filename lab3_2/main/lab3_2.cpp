#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DFRobot_LCD.h"

#define SDA_PIN 10
#define SCL_PIN 8

void app_main(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_PIN;
    conf.scl_io_num = SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    DFRobot_LCD lcd(16, 2);

    while (1) {
        lcd.init();
        lcd.setRGB(0, 255, 0);
        lcd.printstr("Hello CSE121!");
        lcd.cursor(0, 1);
        lcd.printstr("Loro");

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
