#include "common.h"
#include "i2c.h"
#include "pit.h"
#include <util/twi.h>

/* Init I2C IP */
void i2c_init(){
	TWI0.MBAUD = ((F_CPU/100000)-16)/2;
	TWI0.MCTRLA = TWI_ENABLE_bm;
	TWI0.CTRLA = TWI_SDAHOLD_300NS_gc;
}

/* Send start condition */
static void i2c_start(uint8_t addr){
	TWI0.MCTRLB |= TWI_FLUSH_bm;
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
	TWI0.MADDR = addr;
	while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm)) break_out(1);
}
static void i2c_addr(uint8_t addr){
	TWI0.MADDR = addr;
	while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm)) break_out(1);
}
/* Send stop condition */
static void i2c_stop(void){
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

/* Send byte */
static void i2c_write(uint8_t data){
	TWI0.MDATA = data;
	while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm)) break_out(1);
}

/* Read byte with ACK */
static uint8_t i2c_readACK(){
	TWI0.MCTRLB &= ~TWI_ACKACT_ACK_gc;
	while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm)) break_out(1);
	return TWI0.MDATA;
}

/* Read byte with NACK */
static uint8_t i2c_readNACK(){
	TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
	while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm)) break_out(1);
	return TWI0.MDATA;
}

/* Get status */
static uint8_t i2c_status(){
	return TWI0.MSTATUS & (TWI_ARBLOST_bm | TWI_BUSERR_bm);
}

/* Send byte to i2c device */
int i2c_send(uint8_t addr, uint8_t reg, uint8_t data){
	i2c_start(addr);
	if(i2c_status())
		return i2c_status();
	i2c_write(reg);
	if(i2c_status())
		return i2c_status();
	i2c_write(data);
	if(i2c_status())
		return i2c_status();
	i2c_stop();
	return 0;
}

/* Read byte from i2c device */
int i2c_read(uint8_t addr, uint8_t reg, uint8_t* data){
	i2c_start(addr);
	if(i2c_status())
		return i2c_status();
	i2c_write(reg);
	if(i2c_status())
		return i2c_status();
	i2c_start(addr | 0x01);
	if(i2c_status())
		return i2c_status();
	*data = i2c_readNACK();
	if(i2c_status())
		return i2c_status();
	i2c_stop();
	return 0;
}
