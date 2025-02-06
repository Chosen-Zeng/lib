#ifndef __BRT_INST_H
#define __BRT_INST_H

#define BRT_VAL_READ 0x01 // DLC: 1
#define BRT_ID_SET 0x02   // DLC: 1

#define BRT_CAN_BAUD_SET 0x03 // DLC: 1
#define BRT_CAN_BAUD_500Kbps 0x00
#define BRT_CAN_BAUD_1Mbps 0x01
#define BRT_CAN_BAUD_250Kbps 0x02
#define BRT_CAN_BAUD_125Kbps 0x03
#define BRT_CAN_BAUD_100Kbps 0x04

#define BRT_MODE_SET 0x04 // DLC: 1
#define BRT_MODE_CHECK 0x00
#define BRT_MODE_DPS 0x02
#define BRT_MODE_VAL 0xAA

#define BRT_ATD_SET 0x05 // DLC: 2, auto transmit delay

#define BRT_POS_0_SET 0x06 // DLC: 1

#define BRT_INC_DIRCT_SET 0x07 // DLC: 1
#define BRT_INC_DIRCT_CW 0x00
#define BRT_INC_DIRCT_CCW 0x01

#define BRT_DPS_READ 0x0A      // DLC: 1
#define BRT_DPS_SMPLT_SET 0x0B // DLC: 2

#define BRT_POS_MID_SET 0x0C  // DLC: 1
#define BRT_POS_CURR_SET 0x0D // DLC: 4
// 0x0E

// single-lap only
#ifdef BRT_LAP_SGL
#define BRT_VAL_MPL_READ 0x08 // DLC: 1
#define BRT_VAL_LAP_READ 0x09 // DLC: 1

// multi-lap only
#elif defined BRT_LAP_MPL
#define BRT_5LAP_SET 0x0F // DLC: 1
#endif

#endif