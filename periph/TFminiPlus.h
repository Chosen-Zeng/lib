#ifndef __TFminiPlus_H
#define __TFminiPlus_H

#include "usr.h"

#ifdef TFminiPlus_NUM

#define TFminiPlus_PREAMBLE_RECV 0x5959

typedef struct
{
    unsigned short dist_cm, strength;
    float temp;
} TFminiPlus_t;
extern TFminiPlus_t TFminiPlus[TFminiPlus_NUM];

extern unsigned char TFminiPlus_RxData[TFminiPlus_NUM][9];

#endif
#endif