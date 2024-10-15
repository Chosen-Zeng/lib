#ifndef __BRT38_H
#define __BRT38_H

#define DEBUG

#define BRT38_LAP // type selection: BRT38_SGL_LAP/BRT38_MPL_LAP
#if defined BRT38_LAP_SGL || defined BRT38_LAP_MPL

#define BRT38_MODE_VAL // mode selection: BRT38_MODE_CHECK/BRT38_MODE_DPS/BRT38_MODE_VAL
#if defined BRT38_MODE_CHECK || defined BRT38_MODE_DPS || defined BRT38_MODE_VAL

#define RES_BRT38 32768  // define resolution
#ifdef RES_BRT38
#define fANGLE_BRT38 (360.f / RES_BRT38)

#ifdef BRT38_LAP_MPL
//#define LAP_BRT38 50    // define lap
#ifdef LAP_BRT38
#else
#error No lap defined.
#endif
#endif

#else
#error No resolution defined.
#endif

#else
#warning No mode selected.
#endif

#else
#warning No type selected.
#endif


#ifdef DEBUG
#define BRT38_ERROR 0
#define BRT38_SUCCESS 1
#endif

void BRT38_Init(FDCAN_HandleTypeDef *hfdcan, uint8_t addr);
void BRT38_SendData(FDCAN_HandleTypeDef* hfdcan, uint8_t addr, uint8_t func, uint32_t data, uint8_t datalength);

#endif

