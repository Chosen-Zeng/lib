#ifndef __BRT_H
#define __BRT_H

#include "main.h"

#define BRT_NUM 2
#define BRT_OFFSET 4


#define BRT_LAP_MPL // type selection: BRT_LAP_SGL/BRT_LAP_MPL
#if defined BRT_LAP_SGL || defined BRT_LAP_MPL

#define BRT_RES 4096 // define resolution
#ifdef BRT_RES
#define BRT_fANGLE (360.F / BRT_RES)

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

#ifdef DEBUG
#define BRT_ERROR 0
#define BRT_SUCCESS 1
#endif

typedef struct
{
    uint8_t length;
    uint8_t addr;
    uint8_t func;
    uint8_t data[4];
} BRT_msg_t;

extern float BRT_angle[BRT_NUM];
extern BRT_msg_t BRT_msg_ctrl;

void BRT_Init(void *CAN_handle, uint8_t ID);
void BRT_SendCmd(void *CAN_handle, uint8_t ID, uint8_t BRT_func, uint32_t data);

#endif
