#ifndef __BRT38_H
#define __BRT38_H

#define DEBUG_F

#define BRT38_LAP // type selection: BRT38_SGL_LAP/BRT38_MPL_LAP
#if defined BRT38_LAP_SGL || defined BRT38_LAP_MPL

#define BRT38_MODE_VAL // mode selection: BRT38_MODE_CHECK/BRT38_MODE_DPS/BRT38_MODE_VAL
#if defined BRT38_MODE_CHECK || defined BRT38_MODE_DPS || defined BRT38_MODE_VAL

#define BRT38_RES 32768  // define resolution
#ifdef BRT38_RES
#define BRT38_fANGLE (360.F / BRT38_RES)

#ifdef BRT38_LAP_MPL
//#define LAP_BRT38 50    // define lap
#ifdef BRT38_LAP
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


#ifdef DEBUG_T
#define BRT38_ERROR 0
#define BRT38_SUCCESS 1
#endif

void BRT38_Init(FDCAN_HandleTypeDef *hfdcan, uint8_t addr);
void BRT38_SendData(FDCAN_HandleTypeDef* hfdcan, uint8_t addr, uint8_t func, uint32_t data, uint8_t datalength);

#endif

