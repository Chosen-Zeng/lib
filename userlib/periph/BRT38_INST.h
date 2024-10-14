#ifndef __BRT38_INST_H
#define __BRT38_INST_H

#define BRT38_VAL_READ 0x01
#define BRT38_ID_SET 0x02

#define BRT38_CAN_BAUD_SET 0x03
#define BRT38_CAN_BAUD_500Kbps 0x00
#define BRT38_CAN_BAUD_1Mbps 0x01
#define BRT38_CAN_BAUD_250Kbps 0x02
#define BRT38_CAN_BAUD_125Kbps 0x03
#define BRT38_CAN_BAUD_100Kbps 0x04

#define BRT38_MODE_SET 0x04

#ifdef BRT38_MODE_CHECK
#undef BRT38_MODE_CHECK
#define BRT38_MODE_CHECK 0x00
#endif
#ifdef BRT38_MODE_DPS
#undef BRT38_MODE_DPS
#define BRT38_MODE_DPS 0x02
#endif
#ifdef BRT38_MODE_VAL
#undef BRT38_MODE_VAL
#define BRT38_MODE_VAL 0xAA
#endif

#define BRT38_ATD_SET 0x05 // auto transmit delay

#define BRT38_ZP_SET 0x06  // zero point

#define BRT38_INC_DIRCT_SET 0x07
#define BRT38_INC_DIRCT_CW 0x00
#define BRT38_INC_DIRCT_CCW 0x01

#define BRT38_DPS_READ 0x0A
#define BRT38_DPS_SMPLT_SET 0x0B

#define BRT38_MIDP_SET 0x0C
#define BRT38_CURRP_SET 0x0D
//0x0E

//single-lap only
#ifdef BRT38_LAP_SGL
#define BRT38_VAL_MPL_READ 0x08
#define BRT38_VAL_LAP_READ 0x09
#define BRT38_5LAP_SET 0x0F
#endif

#endif