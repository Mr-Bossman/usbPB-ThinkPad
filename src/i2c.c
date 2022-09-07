#include "common.h"
#include "i2c.h"
#include "watchdog.h"
#include <util/twi.h>

/* Init I2C IP */
void i2c_init(){
	PORTC |= (1<<PC4)|(1<<PC5);
	// Set SCL to 400kHz
	TWSR = 0x00;
	TWBR = 0x0C;//((F_CPU/SCL_CLOCK)-16)/2; //TODO: Check this
	// Enable TWI
	TWCR = (1<<TWEN);
}

/* Send start condition */
static void i2c_start(){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0) break_out(1);
}

/* Send stop condition */
static void i2c_stop(void){
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

/* Send byte */
static void i2c_write(uint8_t data){
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0) break_out(1);
}

/* Read byte with ACK */
static uint8_t i2c_readACK(){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while ((TWCR & (1<<TWINT)) == 0) break_out(1);
    return TWDR;
}

/* Read byte with NACK */
static uint8_t i2c_readNACK(){
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0) break_out(1);
    return TWDR;
}

/* Get status */
static uint8_t i2c_status(){
    uint8_t status;
    status = TWSR & 0xF8;
    return status;
}

/* Send byte to i2c device */
int i2c_send(uint8_t addr, uint8_t reg, uint8_t data){
	i2c_start();
	if(i2c_status() != TW_START)
		return i2c_status();
	i2c_write(addr);
	if(i2c_status() != TW_MT_SLA_ACK)
		return i2c_status();
	i2c_write(reg);
	if(i2c_status() != TW_MT_DATA_ACK)
		return i2c_status();
	i2c_write(data);
	if(i2c_status() != TW_MT_DATA_ACK)
		return i2c_status();
	i2c_stop();
	return 0;
}

/* Read byte from i2c device */
int i2c_read(uint8_t addr, uint8_t reg, uint8_t* data){
	i2c_start();
	if(i2c_status() != TW_START)
		return i2c_status();
	i2c_write(addr);
	if(i2c_status() != TW_MT_SLA_ACK)
		return i2c_status();
	i2c_write(reg);
	if(i2c_status() != TW_MT_DATA_ACK)
		return i2c_status();
	i2c_start();
	if(i2c_status() != TW_REP_START)
		return i2c_status();
	i2c_write(addr | 0x01);
	if(i2c_status() != TW_MR_SLA_ACK)
		return i2c_status();
	*data = i2c_readNACK();
	if(i2c_status() != TW_MR_DATA_NACK)
		return i2c_status();
	i2c_stop();
	return 0;
}
