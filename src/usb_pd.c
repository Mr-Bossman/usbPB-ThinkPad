#include "common.h"
#include "usb_pd.h"
#include "fusb302.h"

enum  pd_supply_type { fixed = 0, battery = 1, variable = 2, augmented = 3 };

/// Power source capability
struct source_capability {
    /// Supply type (fixed, batttery, variable etc.)
    enum pd_supply_type supply_type;
    /// Position within message (don't touch)
    uint8_t obj_pos;
    /// Maximum current (in mA)
    uint16_t max_current;
    /// Voltage (in mV)
    uint16_t voltage;
} source_caps[10];

uint8_t num_source_caps = 0;

void handle_src_cap_msg(uint16_t header, const uint8_t* payload)
{
    int n = num_obj(header);

    num_source_caps = 0;

    for (int obj_pos = 0; obj_pos < n; obj_pos++) {
        if (num_source_caps >= sizeof(source_caps) / sizeof(source_caps[0]))
            break;

        enum pd_supply_type type = (enum pd_supply_type)(payload[3] >> 6);
        uint16_t max_current = 0;
        uint16_t voltage = 0;

        if (type == fixed) {
            voltage = ((payload[2] & 0x0f) * 64 + (payload[1] >> 2)) * 50;
            max_current = ((payload[1] & 0x03) * 256 + payload[0]) * 10;
        }
	uart_send(voltage>>8);
	uart_send(voltage);
	uart_send(0x55);
        source_caps[num_source_caps] = (struct source_capability){
            .supply_type = type,
            .obj_pos = (uint8_t)(obj_pos + 1),
            .max_current = max_current,
            .voltage = voltage,
        };
        num_source_caps++;

        payload += 4;
    }
}


void usb_pd_handle_message(uint16_t header, const uint8_t* payload){
	uint8_t type = msg_type(header);
	switch (type) {
		case data_source_capabilities:
			handle_src_cap_msg(header, payload);
		break;
    		default:
		break;
    	}
}
