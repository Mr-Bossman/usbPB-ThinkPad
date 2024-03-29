#include "common.h"
#include "fusb302.h"
#include "i2c.h"
#include "uart.h"
#include "usb_pd.h"
#include "pit.h"
#include "printf_uart.h"

static volatile uint8_t state = PD_STATE_IDLE;
static uint8_t next_message_id = 0;

/* Get state */
uint8_t fusb302_get_state()
{
	return state;
}

/* Init FUSB302 */
void fusb302_init(){
	state = PD_STATE_IDLE;
	fusb302_write(REG_RESET, REG_RESET_SW_RESET | REG_RESET_PD_RESET);
	_delay_ms(10);
	fusb302_write(REG_POWER, REG_POWER_PWR_ALL);
	fusb302_write(REG_SWITCHES0, 0x00);
	fusb302_write(REG_MASK, ~(REG_MASK_ACTIVITY));
	fusb302_write(REG_MASKA, 0xff);
	fusb302_write(REG_MASKB, REG_MASKB_GCRCSENT);
	fusb302_start_dual();
	pit_IE(0);
}

/* Get device ID */
int fusb302_id(uint8_t* id){
	return fusb302_read(REG_DEVICE_ID, id);
}

/* Reset FUSB302 */
int fusb302_reset(){
	return fusb302_write(REG_RESET, REG_RESET_PD_RESET);
}

/* Start Sink */
int fusb302_start_dual(){
	/* BMC threshold: 1.35V with a threshold of 85mV */
	if(fusb302_write(REG_SLICE, REG_SLICE_SDAC_HSYS_85 | 0x20)) {
		return 1;
	}
	fusb302_write(REG_MASKA, ~REG_MASKA_TOGDONE);
	fusb302_write(REG_CONTROL2, (REG_CONTROL2_MODE_DRP << REG_CONTROL2_MODE_POS) | REG_CONTROL2_TOGGLE | REG_CONTROL2_RD_ONLY);
	fusb302_write(REG_CONTROL3, REG_CONTROL3_AUTO_RETRY | (3 << REG_CONTROL3_N_RETRIES_POS));
	fusb302_write(REG_MEASURE,0b100110); // 1.6v for Default USB power below \/
	fusb302_write(REG_CONTROL0, REG_CONTROL0_HOST_CUR_USB);
	state = PD_STATE_SNK_2_0;
	return 0;
}

/* Check cc state */
static int fusb302_check_cc_state(){
	uint8_t val;
	if(fusb302_read(REG_STATUS1A, &val)) {
		return -1;
	}
	if((val & 0x38) == REG_STATUS1A_TOGSS_SNK1 << REG_STATUS1A_TOGSS_POS) {
		return 0;
	} else if((val & 0x38) == REG_STATUS1A_TOGSS_SNK2 << REG_STATUS1A_TOGSS_POS) {
		return 1;
	}
	if((val & 0x38) == REG_STATUS1A_TOGSS_SRC1 << REG_STATUS1A_TOGSS_POS) {
		return 2;
	} else if((val & 0x38) == REG_STATUS1A_TOGSS_SRC2 << REG_STATUS1A_TOGSS_POS) {
		return 3;
	}
	/* Not in clecked state */
	return -1;
}

/* Start source */
static int fusb302_start_src(int cc){
	uint8_t val;
	cc -= 2;
	fusb302_write(REG_SWITCHES0, (cc?REG_SWITCHES0_MEAS_CC2:REG_SWITCHES0_MEAS_CC1) | (cc?REG_SWITCHES0_VCONN_CC1:REG_SWITCHES0_VCONN_CC2) | REG_SWITCHES0_CC2_PD_EN | REG_SWITCHES0_CC1_PD_EN);
	fusb302_write(REG_SWITCHES1, REG_SWITCHES1_SPECREV0 | REG_SWITCHES1_AUTO_GCRC | (cc?REG_SWITCHES1_TXCC2_EN:REG_SWITCHES1_TXCC1_EN));
	fusb302_write(REG_MASK, ~(REG_MASK_ACTIVITY | REG_MASK_BC_LVL));
	state = PD_STATE_SRC;
	return 0;
}

/* Establish usb pd wait */
static int fusb302_establish_pd_wait(int cc){
	fusb302_write(REG_CONTROL2, (REG_CONTROL2_MODE_DRP << REG_CONTROL2_MODE_POS));
	fusb302_write(REG_MASK, ~(REG_MASK_ACTIVITY | REG_MASK_CRC_CHK));
	fusb302_write(REG_SWITCHES0, (cc?REG_SWITCHES0_MEAS_CC2:REG_SWITCHES0_MEAS_CC1) | (cc?REG_SWITCHES0_VCONN_CC1:REG_SWITCHES0_VCONN_CC2) | REG_SWITCHES0_CC2_PD_EN | REG_SWITCHES0_CC1_PD_EN);
	fusb302_write(REG_SWITCHES1, REG_SWITCHES1_SPECREV0 | REG_SWITCHES1_AUTO_GCRC | (cc?REG_SWITCHES1_TXCC2_EN:REG_SWITCHES1_TXCC1_EN));
	fusb302_write(REG_CONTROL0, 0x00);
	state = PD_STATE_SNK_WAIT;
	pit_IE(1);
	start_timer();
	return 0;
}

/* Read usbPB data */
static int fusb302_read_usbpb(uint8_t* data, size_t sz){
	for(size_t i = 0; i < sz; i++)
		if(fusb302_read(REG_FIFOS, &data[i]))
			return 1;
	return 0;
}

/* Write usbPB data */
static int fusb302_write_usbpb(uint8_t* data, size_t sz){
	for(size_t i = 0; i < sz; i++)
		if(fusb302_write(REG_FIFOS, data[i]))
			return 1;
	return 0;
}

/* Create and send message */
void fusb302_send_message(uint16_t header, const uint8_t* payload)
{
	header |= (next_message_id << 8);
	int payload_len = num_obj(header) * 4;

	uint8_t buf[40];

	// Create token stream
	buf[0] = sop1;
	buf[1] = sop1;
	buf[2] = sop1;
	buf[3] = sop2;
	buf[4] = packsym | (payload_len + 2);
	buf[5] = header & 0xff;
	buf[6] = header >> 8;
	if (payload_len > 0)
		memcpy(buf + 7, payload, payload_len);
	int n = 7 + payload_len;
	buf[n++] = jam_crc;
	buf[n++] = eop;
	buf[n++] = txoff;
	buf[n++] = txon;

	fusb302_write_usbpb(buf, n);

	next_message_id++;
	if (next_message_id == 8)
		next_message_id = 0;
}

/* Read message */
static int fusb302_read_message(uint16_t* header, uint8_t* payload){
	uint8_t data[3];
	if(fusb302_read_usbpb(data, 3))
		return -1;
	if((data[0]&0xe0) != 0xe0){
		fusb302_write(REG_CONTROL1, REG_CONTROL1_RX_FLUSH);
		return -2;
	}
	header[0] = (data[1] << 8) | data[2];
	uint8_t len = num_obj(header[0]) * 4;
	if(fusb302_read_usbpb(payload, len+4))
		return -3;
	return len;
}

/* Check for message <0 err 0 is no pd 1 is pd*/
static int fusb302_check_for_message(){
	int ret = 0;
	while(1){
		uint8_t val = 0;
		if(fusb302_read(REG_STATUS1, &val))
			return -1;
		if(val&REG_STATUS1_RX_EMPTY)
			break;
		uint16_t header = 0;
		uint8_t payload[256];
		int len = fusb302_read_message(&header, payload);
		if(fusb302_read(REG_STATUS0, &val))
			return -2;
		if((val&REG_STATUS0_CRC_CHK) == 0)
			continue;
		if(msg_type(header) == 0x01)
			continue;
		ret = 1;
		usb_pd_handle_message(header, payload);
	}
	return ret;
}

/* Global fusb302 interrupt handler */
void fusb302_IRQ(void){
	uint8_t intr[4];
	fusb302_read(REG_INTERRUPT, &intr[0]);
	fusb302_read(REG_INTERRUPTA, &intr[1]);
	fusb302_read(REG_INTERRUPTB, &intr[2]);
	fusb302_read(REG_STATUS1A, &intr[3]);
	uart_printf("\n\rstate: %d. interrupts: 0x%x 0x%x 0x%x 0x%x\n\r",state, intr[0], intr[1], intr[2], intr[3]>>3);

	/**/if(intr[0] & REG_INTERRUPT_VBUSOK) {
		uint8_t tmp = 0;
		fusb302_read(REG_STATUS0, &tmp);
		if(!(tmp & REG_STATUS0_VBUSOK)) {
			fusb302_init();
			usb_pd_reset_source_caps();
		}

	}
	if(intr[1]&REG_INTERRUPTA_TX_SUCCESS)
		return;
	if (state == PD_STATE_SNK_2_0) {
		int cc = fusb302_check_cc_state();
		if(cc == -1) {
			fusb302_init();
			return;
		}
		if(cc == 1 || cc == 0)
			fusb302_establish_pd_wait(cc);
		else if (cc == 2 || cc == 3)
			fusb302_start_src(cc);
	} else if (state == PD_STATE_SNK_WAIT) {
		if(fusb302_check_for_message() == 1){
			state = PD_STATE_SNK_PD;
			fusb302_write(REG_MASK, ~(REG_INTERRUPT_VBUSOK));
			fusb302_write(REG_MASKA, 0xff);
			/* Send ping so I dont get shut off */
			usb_pd_request_power(5000,3000);
		}
	} else if (state == PD_STATE_SNK_PD){
		uint8_t val = 0;
		if(fusb302_read(REG_STATUS1, &val))
			return;
		if(!(val&REG_STATUS1_RX_EMPTY))
			fusb302_write(REG_CONTROL1, REG_CONTROL1_RX_FLUSH);
	} else if (state == PD_STATE_SRC) {
		fusb302_init();
	}
}
