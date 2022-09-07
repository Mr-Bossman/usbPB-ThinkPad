#ifndef __USB_PD_H__
#define __USB_PD_H__
#include <stdint.h>
#include "fusb302.h"
#include "i2c.h"

enum {
	ctrl_good_crc = 0x01,
	ctrl_goto_min = 0x02,
	ctrl_accept = 0x03,
	ctrl_reject = 0x04,
	ctrl_ping = 0x05,
	ctrl_ps_ready = 0x06,
	ctrl_get_source_cap = 0x07,
	ctrl_get_sink_cap = 0x08,
	ctrl_dr_swap = 0x09,
	ctrl_pr_swap = 0x0a,
	ctrl_vconn_swap = 0x0b,
	ctrl_wait = 0x0c,
	ctrl_soft_reset = 0x0d,
	ctrl_data_reset = 0x0e,
	ctrl_data_reset_complete = 0x0f,
	ctrl_not_supported = 0x10,
	ctrl_get_source_cap_extended = 0x11,
	ctrl_get_status = 0x12,
	ctrl_fr_swap = 0x13,
	ctrl_get_pps_status = 0x14,
	ctrl_get_country_codes = 0x15,
	ctrl_get_sink_cap_extended = 0x16,
	data_source_capabilities = 0x81,
	data_request = 0x82,
	data_bist = 0x83,
	data_sink_capabilities = 0x84,
	data_battery_status = 0x85,
	data_alert = 0x86,
	data_get_country_info = 0x87,
	data_enter_usb = 0x88,
	data_vendor_defined = 0x8f
};

enum  pd_supply_type { fixed = 0, battery = 1, variable = 2, augmented = 3 };

enum  {
	txon = 0xa1,
	sop1 = 0x12,
	sop2 = 0x13,
	sop3 = 0x1b,
	reset1 = 0x15,
	reset2 = 0x16,
	packsym = 0x80,
	jam_crc = 0xff,
	eop = 0x14,
	txoff = 0xfe
};

/* Power source capability */
struct source_capability {
	/* Supply type (fixed, batttery, variable etc.) */
	enum pd_supply_type supply_type;
	/* Position within message (don't touch) */
	uint8_t obj_pos;
	/*Maximum current (in mA) */
	uint16_t max_current;
	/* Voltage (in mV) */
	uint16_t voltage;
};

static inline int fusb302_write(uint8_t reg, uint8_t data){
	return i2c_send(FUSB302_I2C_SLAVE_ADDR, reg, data);
}

static inline int fusb302_read(uint8_t reg, uint8_t* data){
	return i2c_read(FUSB302_I2C_SLAVE_ADDR, reg, data);
}

static inline bool has_extened(uint16_t header){
	return (header & 0x8000) != 0;
}

static inline int num_obj(uint16_t header){
	return (header >> 12) & 0x7;
}

static inline int msg_id(uint16_t header){
	return (header >> 9) & 0x7;
}

static inline uint8_t msg_type(uint16_t header){
	return (num_obj(header) != 0) << 7 | (header & 0x1f);
}

static inline uint16_t create_header(uint8_t msg_type, int num_obj){
		return ((num_obj & 0x07) << 12) | (msg_type & 0x1f) | 0x40;
}

void usb_pd_handle_message(uint16_t header, const uint8_t* payload);
void usb_pd_request_power(uint16_t voltage, uint16_t max_current);
uint8_t usb_pd_get_source_caps(struct source_capability** caps);
void usb_pd_reset_source_caps();

#endif /* __USB_PD_H__ */
