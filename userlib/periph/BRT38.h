#ifndef __BRT38_H
#define __BRT38_H

#define _DEBUG

#define BRT38_LAP_SGL // type selection: BRT38_LAP_SGL/BRT38_LAP_MPL
#if defined BRT38_LAP_SGL || defined BRT38_LAP_MPL

#define BRT38_RES 4096 // define resolution
#ifdef BRT38_RES
#define BRT38_fANGLE (360.F / BRT38_RES)

#ifdef BRT38_LAP_MPL
#define BRT38_LAP 50 // define lap
#ifndef BRT38_LAP
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
#define BRT38_ERROR 0
#define BRT38_SUCCESS 1
#endif

typedef struct
{
    uint8_t length;
    uint8_t addr;
    uint8_t func;
    uint8_t data[4];
} BRT38_t;

void BRT38_Init(void *CAN_handle, uint8_t ID);

#endif
