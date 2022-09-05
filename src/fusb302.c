#include "common.h"
#include "fusb302.h"
#include "i2c.h"
static uint8_t state = 0;

static inline int fusb302_write(uint8_t reg, uint8_t data){
	return i2c_send(FUSB302_I2C_SLAVE_ADDR, reg, data);
}

static inline int fusb302_read(uint8_t reg, uint8_t* data){
	return i2c_read(FUSB302_I2C_SLAVE_ADDR, reg, data);
}
/* Init FUSB302 */
void fusb302_init(){
	fusb302_write(REG_RESET, REG_RESET_SW_RESET);
	fusb302_write(REG_SWITCHES0, 0x00);
	fusb302_write(REG_MASK, 0xff);
	fusb302_write(REG_MASKA, 0xff);
	fusb302_write(REG_MASKB, REG_MASKB_GCRCSENT);
	fusb302_write(REG_POWER, REG_POWER_PWR_ALL);
}

/* Get device ID */
uint8_t fusb302_id(){
	uint8_t val;
	fusb302_read(REG_DEVICE_ID, &val);
	return val;
}

/* Start measurement */
int fusb302_start_measurement(bool cc) {
	return fusb302_write(REG_SWITCHES0, (cc?REG_SWITCHES0_MEAS_CC2:REG_SWITCHES0_MEAS_CC1) | REG_SWITCHES0_CC2_PD_EN |REG_SWITCHES0_CC1_PD_EN);
}

/* Reset FUSB302 */
int fusb302_reset(){
	return fusb302_write(REG_RESET, REG_RESET_PD_RESET);
}

/* Start Sink */
int fusb302_start_sink(){
	/* BMC threshold: 1.35V with a threshold of 85mV */
	if(fusb302_write(REG_SLICE, REG_SLICE_SDAC_HSYS_85 | 0x20)) {
		return 1;
	}
	fusb302_write(REG_MASKA, ~REG_MASKA_TOGDONE);
	fusb302_write(REG_CONTROL2, (REG_CONTROL2_MODE_UFP << REG_CONTROL2_MODE_POS) | REG_CONTROL2_TOGGLE);
	fusb302_write(REG_CONTROL0, REG_CONTROL0_HOST_CUR_USB);
	fusb302_write(REG_CONTROL3, REG_CONTROL3_AUTO_RETRY | (3 << REG_CONTROL3_N_RETRIES_POS));
	state = 1;
	return 0;
}

/* Check mesurement */
int fusb302_check_mesurement(){
	uint8_t val;
	if(fusb302_read(REG_STATUS0, &val)) {
		return 1;
	}
	return 0;
}

/* Check cc state */
int fusb302_check_cc_state(){
	uint8_t val;
	if(fusb302_read(REG_STATUS1A, &val)) {
		return -1;
	}
	if((val & 0x38) == REG_STATUS1A_TOGSS_SNK1 << REG_STATUS1A_TOGSS_POS) {
		return 0;
	} else if((val & 0x38) == REG_STATUS1A_TOGSS_SNK2 << REG_STATUS1A_TOGSS_POS) {
		return 1;
	}
	/* Not in clecked state */
	return -2;
}

/* Establish usb wait */
int fusb302_establish_usb_wait(){
	int cc = fusb302_check_cc_state();
	fusb302_write(REG_CONTROL2, (REG_CONTROL2_MODE_UFP << REG_CONTROL2_MODE_POS));
	fusb302_write(REG_MASK, ~(REG_MASK_ACTIVITY | REG_MASK_CRC_CHK));
	fusb302_start_measurement(cc);
	fusb302_write(REG_SWITCHES1, REG_SWITCHES1_SPECREV0 | REG_SWITCHES1_AUTO_GCRC | (cc?REG_SWITCHES1_TXCC1_EN:REG_SWITCHES1_TXCC2_EN));
	fusb302_write(REG_CONTROL0, 0x00);
	return 0;
}

void fusb302_IRQ(void){
	if(state == 1) {
		fusb302_establish_usb_wait();
		state = 2;
	}
}
