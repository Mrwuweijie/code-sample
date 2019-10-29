#include "hal_i2c.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_delay.h"
#include "app_config.h"
#define NRF_LOG_MODULE_NAME hal_i2c_soft
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#if CONFIG_IIC_ENABLED
#if (CONFIG_IIC_MODE == CONFIG_IIC_MODE_SOFT)

#define IIC_WRITE_BIT	0x00
#define IIC_READ_BIT	0x01

static void i2c_delay(uint8_t delay_count){
	nrf_delay_us(delay_count);
}

/*
*	I2C start signal
*	When SCL is high, SDA high to low.
*/
static void i2c_start(void){
	I2C_SDA_MODE_OUT();
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_delay(1);
	I2C_SDA_0();
	i2c_delay(2);
	I2C_SCL_0();
	i2c_delay(1);
}

/*
*	I2C stop signal
*	When SCL is high, SDA low to high.
*/
static void i2c_stop(void)
{
	I2C_SDA_MODE_OUT();
	i2c_delay(2);
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_delay(2);
	I2C_SDA_1();
}

/*
*	I2C send one byte to slave.
*	Inparm: _ucByte    value to be sent
*/
static void i2c_send_one_byte(uint8_t _ucByte){
	uint8_t i;
	I2C_SDA_MODE_OUT();
	for (i = 0; i < 8; i++){
		if (_ucByte & 0x80){
			I2C_SDA_1();
		}
		else{
			I2C_SDA_0();
		}
		i2c_delay(1);
		I2C_SCL_1();
		i2c_delay(2);	
		I2C_SCL_0();
		_ucByte <<= 1;
		i2c_delay(1);
	}
	i2c_delay(1);
	I2C_SDA_0();
}

/*
*	I2C read one byte from slave.
*	Inparm: NULL
*	Return:	value read from slave.
*/
static uint8_t i2c_read_one_byte(void){
	uint8_t i;
	uint8_t value = 0;
	I2C_SDA_MODE_IN();
	for (i = 0; i < 8; i++){
		i2c_delay(2);
		I2C_SCL_1();
		value = value << 1;
		if (I2C_SDA_READ()){
			value++;
		}
		i2c_delay(2);
		I2C_SCL_0();
	}
	return value;
}

/*
*	I2C wait an ACK from slave.
*	Inparm: NULL
*	Return:	TRUE is receive an ACK, FLASE indicate no response from slave.
*/
static uint8_t i2c_wait_ACK(void){
	bool re;
	I2C_SDA_MODE_IN();
	i2c_delay(1);
	I2C_SCL_1();
	i2c_delay(2);
	if (!I2C_SDA_READ()){
		re = true;
	}
	else{
		re = false;
	}
	I2C_SCL_0();
	i2c_delay(2);
	return re;
}

/*
*	I2C Master send an ACK to slave.
*	Inparm: NULL.
*	Return:	NULL.
*/
static void i2c_send_ACK(void){
	I2C_SDA_MODE_OUT();
	i2c_delay(1);
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_delay(2);
	I2C_SCL_0();
	i2c_delay(2);
}

/*
*	I2C send a NACK to slave.
*	Inparm: NULL.
*	Return:	NULL.
*/
static void i2c_send_NACK(void){
	I2C_SDA_MODE_OUT();
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_delay(2);
	I2C_SCL_0();
}

ret_code_t hal_i2c_init(void){
	nrf_gpio_cfg_output(CONFIG_IIC_SCL);
	nrf_gpio_cfg_output(CONFIG_IIC_SDA);
	nrf_delay_us(1);
	return NRF_SUCCESS;
}

void hal_i2c_uninit(void)
{
	nrf_gpio_cfg_input(CONFIG_IIC_SCL, NRF_GPIO_PIN_NOPULL);
	nrf_delay_us(1);
	nrf_gpio_cfg_input(CONFIG_IIC_SDA, NRF_GPIO_PIN_NOPULL);
	nrf_delay_us(1);
}


ret_code_t hal_i2c_write_single(uint8_t slave_addr, uint8_t reg_addr, uint8_t reg_data){
	hal_i2c_init();
	i2c_start();
	i2c_send_one_byte((slave_addr << 1)|IIC_WRITE_BIT);
	i2c_wait_ACK();
	i2c_send_one_byte(reg_addr);
	i2c_wait_ACK();
	i2c_send_one_byte(reg_data);
	i2c_wait_ACK();
	i2c_stop();
	return NRF_SUCCESS;
}

ret_code_t hal_i2c_write_bulk(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, uint16_t length){
	uint8_t temp_write_data;
	uint16_t i;
	hal_i2c_init();
	i2c_start();
	i2c_send_one_byte((slave_addr << 1)|IIC_WRITE_BIT);
	i2c_wait_ACK();
	i2c_send_one_byte(reg_addr);
	i2c_wait_ACK();
	for(i = 0; i < length; i++){
		temp_write_data = *data ++;
		i2c_send_one_byte(temp_write_data);
		i2c_wait_ACK();
	}
	i2c_stop();
	return NRF_SUCCESS;
}

ret_code_t hal_i2c_read_single(uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data){
	hal_i2c_init();
	i2c_start();
	i2c_send_one_byte((slave_addr << 1)|IIC_WRITE_BIT);
	i2c_wait_ACK();
	i2c_send_one_byte(reg_addr);
	i2c_wait_ACK();
	i2c_start();
	i2c_send_one_byte((slave_addr << 1)|IIC_READ_BIT);
	i2c_wait_ACK();
	*reg_data = i2c_read_one_byte();
	i2c_send_NACK();
	i2c_stop();
	return NRF_SUCCESS;
}

ret_code_t hal_i2c_read_bulk(uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t length){
	uint16_t i;
	hal_i2c_init();
	i2c_start();
	i2c_send_one_byte((slave_addr << 1)|IIC_WRITE_BIT);
	i2c_wait_ACK();
	i2c_send_one_byte(reg_addr);
	i2c_wait_ACK();
	i2c_start();
	i2c_send_one_byte((slave_addr << 1)|IIC_READ_BIT);
	i2c_wait_ACK();
	for(i = 0; i < (length - 1); i++){
		*reg_data ++ = i2c_read_one_byte();
		i2c_send_ACK();
	}
	
	*reg_data ++ = i2c_read_one_byte();
	i2c_send_NACK();
	i2c_stop();
	
	return NRF_SUCCESS;
}

#if CONFIG_PWR_MGMT_ENABLED
bool hal_i2c_wakeup_prepare(bool wakeup){
	NRF_LOG_DEBUG("%s", (uint32_t)__func__);
	nrf_gpio_cfg_input(CONFIG_IIC_SCL, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(CONFIG_IIC_SDA, NRF_GPIO_PIN_PULLUP);
    return true;
}
#endif /* END CONFIG_PWR_MGMT_ENABLED */

#endif /* END CONFIG_IIC_MODE == CONFIG_IIC_MODE_SOFT */
#endif /* END CONFIG_IIC_ENABLED */
