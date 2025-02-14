#include "TFminiPlus.h"

#ifdef TFminiPlus_NUM

unsigned char TFminiPlus_RxData[TFminiPlus_NUM][9];
TFminiPlus_t TFminiPlus[TFminiPlus_NUM];

static unsigned char CheckSum(unsigned char data[], unsigned char len)
{
    unsigned char retval = 0;

    while (len--)
        retval += data[len];

    return retval;
}

void USART2_IRQHandler(void)
{
    USART2->ICR |= 0x10;

    if (*(unsigned short *)TFminiPlus_RxData[0] == TFminiPlus_HEAD_DATA  // data head
        && TFminiPlus_RxData[0][8] == CheckSum(TFminiPlus_RxData[0], 8)) // check sum
    {
        TFminiPlus[0].dist_cm = *(unsigned short *)&TFminiPlus_RxData[0][2];
        TFminiPlus[0].strength = *(unsigned short *)&TFminiPlus_RxData[0][4];
        TFminiPlus[0].temp = *(unsigned short *)&TFminiPlus_RxData[0][6] / 8.f - 256;
    }
}
#endif