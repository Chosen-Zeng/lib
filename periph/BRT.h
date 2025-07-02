#ifndef __BRT_H
#define __BRT_H

#include "usr.h"

#if defined BRT_NUM

#define BRT_LAP_MPL // type selection: BRT_LAP_SGL/BRT_LAP_MPL
#if defined BRT_LAP_SGL || defined BRT_LAP_MPL

#define BRT_RES 4096 // define resolution
#ifdef BRT_RES
#define BRT_fANGLE (360.f / BRT_RES)

#ifdef BRT_LAP_MPL
#define BRT_LAP 24 // define lap
#ifndef BRT_LAP
#error No lap defined.
#endif
#endif

#else
#error No resolution defined.
#endif

#else
#warning No type selected.
#endif

#define BRT_VAL_READ 0x01 // DLC: 1
#define BRT_ID_SET 0x02   // DLC: 1

#define BRT_CAN_BAUDRATE_SET 0x03 // DLC: 1
#define BRT_CAN_BAUDRATE_500Kbps 0x00
#define BRT_CAN_BAUDRATE_1Mbps 0x01
#define BRT_CAN_BAUDRATE_250Kbps 0x02
#define BRT_CAN_BAUDRATE_125Kbps 0x03
#define BRT_CAN_BAUDRATE_100Kbps 0x04

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

#ifdef DEBUG
#define BRT_ERROR 0
#define BRT_SUCCESS 1
#endif

extern float BRT[BRT_NUM];

void BRT_Init(void *CAN_handle, uint8_t ID);
void BRT_SendCmd(void *CAN_handle, uint8_t ID, uint8_t BRT_func, uint32_t data);

#endif
#endif