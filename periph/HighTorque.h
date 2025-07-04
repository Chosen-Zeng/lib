#ifndef __HIGHTORQUE_H
#define __HIGHTORQUE_H

#include "usr.h"

#if defined HIGHTORQUE_ID_OFFSET && defined HIGHTORQUE_NUM

#define HIGHTORQUE_ADDR_BCAST 0x7F

#define HIGHTORQUE_NOP 0x50

#define HIGHTORQUE_ADDR_RE 0x8000

#define HIGHTORQUE_DATA_W 0x00
#define HIGHTORQUE_DATA_R 0x10
#define HIGHTORQUE_DATA_RE 0x20

#define HIGHTORQUE_DATA_TYPE_8b 0b0000
#define HIGHTORQUE_DATA_TYPE_16b 0b0100
#define HIGHTORQUE_DATA_TYPE_32b 0b1000
#define HIGHTORQUE_DATA_TYPE_FLOAT 0b1100

#define HIGHTORQUE_DATA_MODE2 0b0000

#define HIGHTORQUE_REG_MODE 0x00
#define HIGHTORQUE_REG_POS_FDBK 0x01
#define HIGHTORQUE_REG_SPD_FDBK 0x02
#define HIGHTORQUE_REG_TRQ_FDBK 0x03
#define HIGHTORQUE_REG_CURR_Q_FDBK 0x04
#define HIGHTORQUE_REG_CURR_D_FDBK 0x05
#define HIGHTORQUE_REG_VOLT 0x0D
#define HIGHTORQUE_REG_TEMP 0x0E
#define HIGHTORQUE_REG_ERROR 0x0F
#define HIGHTORQUE_REG_PWM_A 0x10
#define HIGHTORQUE_REG_PWM_B 0x11
#define HIGHTORQUE_REG_PWM_C 0x12
#define HIGHTORQUE_REG_VOLT_A 0x14
#define HIGHTORQUE_REG_VOLT_B 0x15
#define HIGHTORQUE_REG_VOLT_C 0x16
#define HIGHTORQUE_REG_FOC_VOLT_ANGLE 0x18
#define HIGHTORQUE_REG_FOC_VOLT_VOLT 0x19
#define HIGHTORQUE_REG_VOLT_D 0x1A
#define HIGHTORQUE_REG_VOLT_Q 0x1B
#define HIGHTORQUE_REG_CURR_D 0x1C
#define HIGHTORQUE_REG_CURR_Q 0x1D
#define HIGHTORQUE_REG_POS_CTRL 0x20
#define HIGHTORQUE_REG_SPD_CTRL 0x21
#define HIGHTORQUE_REG_TRQ_OFFSET_CTRL 0x22
#define HIGHTORQUE_REG_Kp 0x23
#define HIGHTORQUE_REG_Kd 0x24
#define HIGHTORQUE_REG_POS_TRQ_LIMIT 0x25
#define HIGHTORQUE_REG_POS_STOP 0x26
#define HIGHTORQUE_REG_SPD_LIMIT 0x28
#define HIGHTORQUE_REG_ACC_LIMIT 0x29
#define HIGHTORQUE_REG_TRQ_Kp 0x30
#define HIGHTORQUE_REG_TRQ_Ki 0x31
#define HIGHTORQUE_REG_TRQ_Kd 0x32
#define HIGHTORQUE_REG_TRQ_FDFWD 0x33
#define HIGHTORQUE_REG_POS_TRQ_FDBK 0x34
#define HIGHTORQUE_REG_POS_MIN 0x40
#define HIGHTORQUE_REG_POS_MAX 0x41

#define HIGHTORQUE_MODE_STOP 0
#define HIGHTORQUE_MODE_ERROR 1
#define HIGHTORQUE_MODE_READY2 2
#define HIGHTORQUE_MODE_READY3 3
#define HIGHTORQUE_MODE_READY4 4
#define HIGHTORQUE_MODE_PWM 5
#define HIGHTORQUE_MODE_VOLT 6
#define HIGHTORQUE_MODE_FOC_VOLT 7
#define HIGHTORQUE_MODE_DQ_VOLT 8
#define HIGHTORQUE_MODE_DQ_CURR 9
#define HIGHTORQUE_MODE_POS 10
#define HIGHTORQUE_MODE_TIMEOUT 11
#define HIGHTORQUE_MODE_0SPD 12
#define HIGHTORQUE_MODE_RANGE 13
#define HIGHTORQUE_MODE_INDUCTOR_MEAS 14
#define HIGHTORQUE_MODE_BRAKE 15

#define HIGHTORQUE_NO_LIMIT_CHAR 0x80
#define HIGHTORQUE_NO_LIMIT_SHORT 0x8000
#define HIGHTORQUE_NO_LIMIT_INT 0x80000000
#define HIGHTORQUE_NO_LIMIT_FLOAT 1.f / 0

typedef const struct
{
    float trq_k, trq_d;
} HighTorque_motor_t;

typedef struct
{
    struct
    {
        float pos, spd, trq, Kp, Kd;
    } ctrl;
    struct
    {
        float pos, spd, trq, temp;
    } fdbk;
} HighTorque_t;
extern HighTorque_t HighTorque[HIGHTORQUE_NUM + 1];

void HighTorque_SetMixParam_f(void *FDCAN_handle, unsigned char ID);
void HighTorque_SwitchMode(void *FDCAN_handle, unsigned char ID, unsigned char HIGHTORQUE_MODE);
void HighTorque_SetSpdLimit(void *FDCAN_handle, unsigned char ID, float spd, float acc);

#endif
#endif