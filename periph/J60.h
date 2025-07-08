#ifndef __J60_H
#define __J60_H

#include "usr.h"

#ifdef J60_NUM

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

#define J60_MOTOR_DISABLE 0x02   // DLC: 0
#define J60_MOTOR_ENABLE 0x04    // DLC: 0
#define J60_MOTOR_CONTROL 0x08   // DLC: 8, null data
#define J60_GET_CONFIG 0x30      // DLC: 0
#define J60_SET_CAN_TIMEOUT 0x12 // DLC: 1
#define J60_SET_BANDWIDTH 0x14   // DLC: 2
#define J60_SAVE_CONFIG 0x20     // DLC: 0
#define J60_GET_STATUSWORD 0x2E  // DLC: 0
#define J60_ERROR_RESET 0x22     // DLC: 0
#define J60_SET_0POS 0x0C        // DLC: 0

#define J60_MSG_SEND 0
#define J60_MSG_RECV 1

typedef struct
{
    const unsigned char ID;

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

void J60_SendCmd(CAN_handle_t *const CAN_handle, const unsigned char arrID, const unsigned short J60_CMD, float data);
void J60_Init(CAN_handle_t *const CAN_handle, const unsigned char arrID);

bool J60_MsgHandler(const unsigned CAN_ID, const unsigned char arrID, const unsigned char RxData[8]);

#endif
#endif