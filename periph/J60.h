#ifndef __J60_H
#define __J60_H

#include "user.h"

#if defined J60_NUM && defined J60_ID_OFFSET

// add offset and take the term as unsigned one then scale the val
#define J60_fPOS_W (80 * R2D / 65535)
#define J60_fPOS_R (80 * R2D / 1048575)
#define J60_fSPD_W (80 * R2D / 16383)
#define J60_fSPD_R (80 * R2D / 1048575)
#define J60_fKd (51.f / 255)
#define J60_fTRQ (80.f / 65535)
#define J60_fTEMP (220.f / 127)
#define J60_fCURR_LIMIT (40.f / 65535)

#define J60_TEMP_FLAG_MOSFET 0
#define J60_TEMP_FLAG_MOTOR 1

#define J60_TEMP_OFFSET -20

#define J60_POS_LIMIT (40 * R2D)
#define J60_SPD_LIMIT (40 * R2D)
#define J60_Kp_LIMIT 1023 // non-ABS limit
#define J60_Kd_LIMIT 51   // non-ABS limit
#define J60_TRQ_LIMIT 40
#define J60_CAN_TIMEOUT_LIMIT 255 // non-ABS limit
#define J60_BANDWIDTH_LIMIT 65535 // non-ABS limit

#define J60_MOTOR_DISABLE (0x02 << 4)   // DLC: 0
#define J60_MOTOR_ENABLE (0x04 << 4)    // DLC: 0
#define J60_MOTOR_CONTROL (0x08 << 4)   // DLC: 8, null data
#define J60_GET_CONFIG (0x30 << 4)      // DLC: 0
#define J60_SET_CAN_TIMEOUT (0x12 << 4) // DLC: 1
#define J60_SET_BANDWIDTH (0x14 << 4)   // DLC: 2
#define J60_SAVE_CONFIG (0x20 << 4)     // DLC: 0
#define J60_GET_STATUSWORD (0x2E << 4)  // DLC: 0
#define J60_ERROR_RESET (0x22 << 4)     // DLC: 0
#define J60_SET_0POS (0x0C << 4)        // DLC: 0

typedef struct
{
    struct
    {
        float pos, spd, Kp, Kd, trq;
    } ctrl;
    struct
    {
        float pos, spd, trq;
        struct
        {
            float temp_MOSFET, temp_motor
        } temp;
    } fdbk;
} J60_t;

extern J60_t J60[J60_NUM];

void J60_SendCmd(void *CAN_handle, unsigned char ID, unsigned short J60_CMD, float data);
void J60_Init(void *CAN_handle, unsigned char ID);

#endif
#endif