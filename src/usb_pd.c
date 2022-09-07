#include "common.h"
#include "usb_pd.h"
#include "fusb302.h"

struct source_capability source_caps[10];

volatile uint8_t num_source_caps = 0;

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
			request_power(5000,1000);
		break;
    		default:
		break;
    	}
}

void request_power(uint16_t voltage, uint16_t max_current)
{
    // Lookup object position by voltage
    int obj_pos = -1;
    for (int i = 0; i < num_source_caps; i++)
        if (source_caps[i].voltage == voltage)
            obj_pos = source_caps[i].obj_pos;
    if (obj_pos == -1)
        return; // no match

    // Create 'request' message
    const uint8_t no_usb_suspend = 1;
    const uint8_t usb_comm_capable = 2;
    uint8_t payload[4];

    uint16_t curr = (max_current + 5) / 10;
    if (curr > 0x3ff)
        curr = 0x3ff;
    payload[0] = curr & 0xff;
    payload[1] = ((curr >> 8) & 0x03) | ((curr << 2) & 0xfc);
    payload[2] = (curr >> 6) & 0x0f;
    payload[3] = (obj_pos & 0x07) << 4 | no_usb_suspend | usb_comm_capable;
    uint16_t header = create_header(data_request, 1);

    // Send message
   send_message(header, payload);
}
