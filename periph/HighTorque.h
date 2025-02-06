#ifndef __HIGHTORQUE_H
#define __HIGHTORQUE_H

#include "user.h"

#if defined HIGHTORQUE_ID_OFFSET && defined HIGHTORQUE_NUM

#define HIGHTORQUE_NOP 0x50
#define HIGHTORQUE_NO_LIMIT_CHAR 0x80
#define HIGHTORQUE_NO_LIMIT_SHORT 0x8000
#define HIGHTORQUE_NO_LIMIT_INT 0x80000000
#define HIGHTORQUE_NO_LIMIT_FLOAT 1.f / 0

#define HIGHTORQUE_ADDR_SRC (0 << 8)
#define HIGHTORQUE_ADDR_nRE 0
#define HIGHTORQUE_ADDR_RE 0x8000

#define HIGHTORQUE_DATA_W 0x00
#define HIGHTORQUE_DATA_R 0x10
#define HIGHTORQUE_DATA_RE 0x20

#define HIGHTORQUE_DATA_TYPE_8 0b0000
#define HIGHTORQUE_DATA_TYPE_16 0b0100
#define HIGHTORQUE_DATA_TYPE_32 0b1000
#define HIGHTORQUE_DATA_TYPE_FLOAT 0b1100

#define HIGHTORQUE_MODE2 0b0000

#define HIGHTORQUE_MODE_STOP 0
#define HIGHTORQUE_MODE_ERROR 1
#define HIGHTORQUE_MODE_PWM 5
#define HIGHTORQUE_MODE_VOLT 6
#define HIGHTORQUE_MODE_FOC_VOLT 7
#define HIGHTORQUE_MODE_DQ_VOLT 8
#define HIGHTORQUE_MODE_DQ_CURR 9
#define HIGHTORQUE_MODE_POS 10
#define HIGHTORQUE_MODE_TIMEOUT 11
#define HIGHTORQUE_MODE_SPD_ZERO 12
#define HIGHTORQUE_MODE_RANGE 13
#define HIGHTORQUE_MODE_INDUCTOR_MEAS 14
#define HIGHTORQUE_MODE_BRAKE 15

/*typedef const struct
{
    float GR, torque_limit, spd_limit, trq_k_f, trq_b_f; // which torque needs to be converted is unknown
} hightorque_motor_t;*/

typedef struct
{
    float pos, spd, trq_offset, Kp, Kd;
} hightorque_ctrl_t;

typedef struct
{
    float pos, spd, trq;
} hightorque_fdbk_t;

typedef struct
{
    hightorque_ctrl_t ctrl;
    hightorque_fdbk_t fdbk;
} hightorque_t;

// extern hightorque_motor_t HTDW_4538_32_NE, HTDW_5047_36_NE;
extern hightorque_t HighTorque[HIGHTORQUE_NUM];

void HighTorque_SendPosParam_f(void *FDCAN_handle, unsigned char ID);
void HighTorque_Stop(void *FDCAN_handle, unsigned char ID);
void HighTorque_SetSpdLimit(void *FDCAN_handle, unsigned char ID, float spd, float acc);

#endif
#endif