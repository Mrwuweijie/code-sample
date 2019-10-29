#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "app_config.h"

#if (CONFIG_IIC_MODE == CONFIG_IIC_MODE_SOFT)
#define I2C_SCL_1()				nrf_gpio_pin_set(CONFIG_IIC_SCL)
#define I2C_SCL_0()				nrf_gpio_pin_clear(CONFIG_IIC_SCL)
#define I2C_SDA_1()				nrf_gpio_pin_set(CONFIG_IIC_SDA)
#define I2C_SDA_0()				nrf_gpio_pin_clear(CONFIG_IIC_SDA)
#define I2C_SDA_READ()			nrf_gpio_pin_read(CONFIG_IIC_SDA)
#define I2C_SDA_MODE_OUT()		nrf_gpio_cfg_output(CONFIG_IIC_SDA)
#define I2C_SDA_MODE_IN()		nrf_gpio_cfg_input(CONFIG_IIC_SDA, NRF_GPIO_PIN_NOPULL)

#endif



ret_code_t hal_i2c_init(void);
void hal_i2c_uninit(void);
ret_code_t hal_i2c_write_single(uint8_t slave_addr, uint8_t reg_addr, uint8_t reg_data);
ret_code_t hal_i2c_write_bulk(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, uint16_t length);
ret_code_t hal_i2c_read_single(uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data);
ret_code_t hal_i2c_read_bulk(uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t length);
bool hal_i2c_wakeup_prepare(bool wakeup);

#endif /* END _HAL_I2C_H_ */

