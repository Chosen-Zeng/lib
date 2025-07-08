#include "TFminiPlus.h"

#ifdef TFminiPlus_NUM

TFminiPlus_t TFminiPlus[TFminiPlus_NUM];

static unsigned char CheckSum(const unsigned char data[], unsigned char len)
{
    unsigned char retval = 0;
    while (len--)
        retval += data[len];
    return retval;
}

bool TFminiPlus_MsgHandler(const unsigned char RxData[9])
{
    if (TFminiPlus_PREAMBLE_RECV == *(unsigned short *)RxData &&
        CheckSum(RxData, 8) == RxData[8]) // check sum
    {
        TFminiPlus[0].dist_cm = *(unsigned short *)&RxData[2];
        TFminiPlus[0].strength = *(unsigned short *)&RxData[4];
        TFminiPlus[0].temp = *(unsigned short *)&RxData[6] / 8.f - 256;

        return true;
    }
    return false;
}

#else
#error TFminiPlus_NUM undefined
#endif