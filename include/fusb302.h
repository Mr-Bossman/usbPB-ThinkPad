#ifndef __FUSB302_H__
#define __FUSB302_H__

/* Chip Device ID - 302A or 302B */
#define FUSB302_DEVID_302A 0x08
#define FUSB302_DEVID_302B 0x09

/* I2C slave address varies by part number */
/* FUSB302BUCX / FUSB302BMPX */
#define FUSB302_I2C_SLAVE_ADDR 0x44

#define REG_DEVICE_ID  0x01

#define REG_SWITCHES0  0x02
#define REG_SWITCHES0_CC2_PU_EN    (1<<7)
#define REG_SWITCHES0_CC1_PU_EN    (1<<6)
#define REG_SWITCHES0_VCONN_CC2    (1<<5)
#define REG_SWITCHES0_VCONN_CC1    (1<<4)
#define REG_SWITCHES0_MEAS_CC2 (1<<3)
#define REG_SWITCHES0_MEAS_CC1 (1<<2)
#define REG_SWITCHES0_CC2_PD_EN    (1<<1)
#define REG_SWITCHES0_CC1_PD_EN    (1<<0)

#define REG_SWITCHES1  0x03
#define REG_SWITCHES1_POWERROLE    (1<<7)
#define REG_SWITCHES1_SPECREV1 (1<<6)
#define REG_SWITCHES1_SPECREV0 (1<<5)
#define REG_SWITCHES1_DATAROLE (1<<4)
#define REG_SWITCHES1_AUTO_GCRC    (1<<2)
#define REG_SWITCHES1_TXCC2_EN (1<<1)
#define REG_SWITCHES1_TXCC1_EN (1<<0)

#define REG_MEASURE    0x04
#define REG_MEASURE_VBUS       (1<<6)
#define REG_MEASURE_MDAC_MV(mv)    (((mv)/42) & 0x3f)

#define REG_SLICE	    0x05
#define REG_SLICE_SDAC_HSYS_85      (1<<6)
#define REG_SLICE_SDAC_HSYS_170     (2<<6)
#define REG_SLICE_SDAC_HSYS_255     (3<<6)


#define REG_CONTROL0   0x06
#define REG_CONTROL0_TX_FLUSH  (1<<6)
#define REG_CONTROL0_INT_MASK  (1<<5)
#define REG_CONTROL0_HOST_CUR_MASK (3<<2)
#define REG_CONTROL0_HOST_CUR_3A0  (3<<2)
#define REG_CONTROL0_HOST_CUR_1A5  (2<<2)
#define REG_CONTROL0_HOST_CUR_USB  (1<<2)
#define REG_CONTROL0_TX_START  (1<<0)

#define REG_CONTROL1   0x07
#define REG_CONTROL1_ENSOP2DB  (1<<6)
#define REG_CONTROL1_ENSOP1DB  (1<<5)
#define REG_CONTROL1_BIST_MODE2    (1<<4)
#define REG_CONTROL1_RX_FLUSH  (1<<2)
#define REG_CONTROL1_ENSOP2    (1<<1)
#define REG_CONTROL1_ENSOP1    (1<<0)

#define REG_CONTROL2   0x08
/* two-bit field, valid values below */
#define REG_CONTROL2_MODE      (1<<1)
#define REG_CONTROL2_RD_ONLY   (1<<5)
#define REG_CONTROL2_MODE_DFP  (0x3)
#define REG_CONTROL2_MODE_UFP  (0x2)
#define REG_CONTROL2_MODE_DRP  (0x1)
#define REG_CONTROL2_MODE_POS  (1)
#define REG_CONTROL2_TOGGLE    (1<<0)

#define REG_CONTROL3   0x09
#define REG_CONTROL3_SEND_HARDRESET    (1<<6)
#define REG_CONTROL3_BIST_TMODE        (1<<5) /* 302B Only */
#define REG_CONTROL3_AUTO_HARDRESET    (1<<4)
#define REG_CONTROL3_AUTO_SOFTRESET    (1<<3)
/* two-bit field */
#define REG_CONTROL3_N_RETRIES     (1<<1)
#define REG_CONTROL3_N_RETRIES_POS     (1)
#define REG_CONTROL3_N_RETRIES_SIZE    (2)
#define REG_CONTROL3_AUTO_RETRY        (1<<0)

#define REG_MASK       0x0A
#define REG_MASK_VBUSOK        (1<<7)
#define REG_MASK_ACTIVITY      (1<<6)
#define REG_MASK_COMP_CHNG     (1<<5)
#define REG_MASK_CRC_CHK       (1<<4)
#define REG_MASK_ALERT     (1<<3)
#define REG_MASK_WAKE      (1<<2)
#define REG_MASK_COLLISION     (1<<1)
#define REG_MASK_BC_LVL        (1<<0)

#define REG_POWER      0x0B
#define REG_POWER_PWR      (1<<0)  /* four-bit field */
#define REG_POWER_PWR_LOW      0x1 /* Bandgap + Wake circuitry */
#define REG_POWER_PWR_MEDIUM   0x3 /* LOW + Receiver + Current refs */
#define REG_POWER_PWR_HIGH     0x7 /* MEDIUM + Measure block */
#define REG_POWER_PWR_ALL      0xF /* HIGH + Internal Oscillator */

#define REG_RESET      0x0C
#define REG_RESET_PD_RESET     (1<<1)
#define REG_RESET_SW_RESET     (1<<0)

#define REG_MASKA      0x0E
#define REG_MASKA_OCP_TEMP     (1<<7)
#define REG_MASKA_TOGDONE      (1<<6)
#define REG_MASKA_SOFTFAIL     (1<<5)
#define REG_MASKA_RETRYFAIL    (1<<4)
#define REG_MASKA_HARDSENT     (1<<3)
#define REG_MASKA_TX_SUCCESS   (1<<2)
#define REG_MASKA_SOFTRESET    (1<<1)
#define REG_MASKA_HARDRESET    (1<<0)

#define REG_MASKB      0x0F
#define REG_MASKB_GCRCSENT     (1<<0)

#define REG_STATUS0A   0x3C
#define REG_STATUS0A_SOFTFAIL  (1<<5)
#define REG_STATUS0A_RETRYFAIL (1<<4)
#define REG_STATUS0A_POWER     (1<<2) /* two-bit field */
#define REG_STATUS0A_RX_SOFT_RESET (1<<1)
#define REG_STATUS0A_RX_HARD_RESET (1<<0)

#define REG_STATUS1A   0x3D
/* three-bit field, valid values below */
#define REG_STATUS1A_TOGSS     (1<<3)
#define REG_STATUS1A_TOGSS_RUNNING     0x0
#define REG_STATUS1A_TOGSS_SRC1        0x1
#define REG_STATUS1A_TOGSS_SRC2        0x2
#define REG_STATUS1A_TOGSS_SNK1        0x5
#define REG_STATUS1A_TOGSS_SNK2        0x6
#define REG_STATUS1A_TOGSS_AA      0x7
#define REG_STATUS1A_TOGSS_POS     (3)
#define REG_STATUS1A_TOGSS_MASK        (0x7)

#define REG_STATUS1A_RXSOP2DB  (1<<2)
#define REG_STATUS1A_RXSOP1DB  (1<<1)
#define REG_STATUS1A_RXSOP     (1<<0)

#define REG_INTERRUPTA 0x3E
#define REG_INTERRUPTA_OCP_TEMP    (1<<7)
#define REG_INTERRUPTA_TOGDONE (1<<6)
#define REG_INTERRUPTA_SOFTFAIL    (1<<5)
#define REG_INTERRUPTA_RETRYFAIL   (1<<4)
#define REG_INTERRUPTA_HARDSENT    (1<<3)
#define REG_INTERRUPTA_TX_SUCCESS  (1<<2)
#define REG_INTERRUPTA_SOFTRESET   (1<<1)
#define REG_INTERRUPTA_HARDRESET   (1<<0)

#define REG_INTERRUPTB 0x3F
#define REG_INTERRUPTB_GCRCSENT        (1<<0)

#define REG_STATUS0    0x40
#define REG_STATUS0_VBUSOK     (1<<7)
#define REG_STATUS0_ACTIVITY   (1<<6)
#define REG_STATUS0_COMP       (1<<5)
#define REG_STATUS0_CRC_CHK    (1<<4)
#define REG_STATUS0_ALERT      (1<<3)
#define REG_STATUS0_WAKE       (1<<2)
#define REG_STATUS0_BC_LVL1    (1<<1) /* two-bit field */
#define REG_STATUS0_BC_LVL0    (1<<0) /* two-bit field */

#define REG_STATUS1    0x41
#define REG_STATUS1_RXSOP2     (1<<7)
#define REG_STATUS1_RXSOP1     (1<<6)
#define REG_STATUS1_RX_EMPTY   (1<<5)
#define REG_STATUS1_RX_FULL    (1<<4)
#define REG_STATUS1_TX_EMPTY   (1<<3)
#define REG_STATUS1_TX_FULL    (1<<2)

#define REG_INTERRUPT  0x42
#define REG_INTERRUPT_VBUSOK   (1<<7)
#define REG_INTERRUPT_ACTIVITY (1<<6)
#define REG_INTERRUPT_COMP_CHNG    (1<<5)
#define REG_INTERRUPT_CRC_CHK  (1<<4)
#define REG_INTERRUPT_ALERT    (1<<3)
#define REG_INTERRUPT_WAKE     (1<<2)
#define REG_INTERRUPT_COLLISION    (1<<1)
#define REG_INTERRUPT_BC_LVL   (1<<0)

#define REG_FIFOS      0x43


void fusb302_init();
int fusb302_reset();
int fusb302_id(uint8_t *id);
int fusb302_start_dual();
void fusb302_IRQ(void);
uint8_t fusb302_get_state();
void fusb302_send_message(uint16_t header, const uint8_t* payload);

#define PD_STATE_IDLE 0
#define PD_STATE_SNK_2_0 1
#define PD_STATE_SNK_WAIT 2
#define PD_STATE_SNK_PD 3
#define PD_STATE_SRC 4

#endif /* __FUSB302_H */
