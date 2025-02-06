#ifndef __J60_H
#define __J60_H

#include "user.h"

#if defined J60_NUM && defined J60_ID_OFFSET

#define R2D 57.29577951

// add offset and take the term as unsigned one then scale the val
#define J60_fPOS_W (80 * R2D / 65535)
#define J60_fPOS_R (80 * R2D / 1048575)
#define J60_fSPD_W (80 * R2D / 16383)
#define J60_fSPD_R (80 * R2D / 1048575)
#define J60_fKd (51.f / 255)
#define J60_fTORQUE (80.f / 65535)
#define J60_fTEMP (220.f / 127)
#define J60_fCURR_LIMIT (40.f / 65535)

#define J60_TEMP_MOSFET 0
#define J60_TEMP_MOTOR 1

#define J60_POS_LIMIT (40 * R2D)
#define J60_SPD_LIMIT (40 * R2D)
#define J60_Kp_LIMIT 1023 // non-ABS limit
#define J60_Kd_LIMIT 51   // non-ABS limit
#define J60_TORQUE_LIMIT 40
#define J60_TEMP_FLAG_MOSFET 0
#define J60_TEMP_FLAG_MOTOR 1
#define J60_TEMP_OFFSET -20
#define J60_CAN_TIMEOUT_LIMIT 255 // non-ABS limit
#define J60_BANDWIDTH_LIMIT 65535 // non-ABS limit

typedef struct
{
    float pos, spd, Kp, Kd, trq_offset;
} J60_ctrl_t;

typedef struct
{
    float pos, spd, trq;
    uint8_t temp_flag;
    float temp_MOSFET, temp_motor;
} J60_fdbk_t;

typedef struct
{
    J60_ctrl_t ctrl;
    J60_fdbk_t fdbk;
} J60_t;

extern J60_t J60[J60_NUM];

void J60_SendCmd(void *CAN_handle, uint8_t ID, uint16_t J60_CMD, float data);
void J60_Init(void *CAN_handle, uint8_t ID);

#endif
#endif