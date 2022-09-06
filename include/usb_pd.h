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


void usb_pd_handle_message(uint16_t header, const uint8_t* payload);

#endif /* __USB_PD_H__ */
