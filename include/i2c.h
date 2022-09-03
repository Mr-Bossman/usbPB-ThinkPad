#ifndef __I2C_H__
#define __I2C_H__
#define SCL_CLOCK 400000UL

/* Init I2C IP */
void i2c_init();
/* Set I2C slave address */
void i2c_set_slave_address(uint8_t address);
/* Send byte to i2c device */
int i2c_send(uint8_t addr, uint8_t data);
/* Read byte from i2c device */
int i2c_read(uint8_t addr, uint8_t* data);
#endif /* __I2C_H__ */
