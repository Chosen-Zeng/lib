#ifndef __J60_H
#define __J60_H

#define R2D (180 / 3.1415926535897)

// angle: -40 ~ 40 rad
// angular velocity: -40 ~ 40 rad/s
// torque: -40 ~ 40 Nm

// add offset and take the term as unsigned one then scale the val
#define J60_fANGLE_W (80 * R2D / (65536 - 1))
#define J60_fANGLE_R (80 * R2D / (1048576 - 1))
#define J60_fSPD_W (80 * R2D / (16384 - 1))
#define J60_fSPD_R (80 * R2D / (1048576 - 1))
#define J60_fKd (51.F / (256 - 1))
#define J60_fTORQUE (80.F / (65536 - 1))
#define J60_fTEMP (220.F / (128 - 1))
#define J60_fCURR_LIMIT (40.F / (65536 - 1))

#define J60_TEMP_MOSFET 0
#define J60_TEMP_MOTOR 1

#define J60_ANGLE_LIMIT (40 * R2D)
#define J60_SPD_LIMIT (40 * R2D)
#define J60_Kp_LIMIT 1023 // non-ABS limit
#define J60_Kd_LIMIT 51  // non-ABS limit
#define J60_TORQUE_LIMIT 40
#define J60_CAN_TIMEOUT_LIMIT 255 // non-ABS limit
#define J60_BANDWIDTH_LIMIT 65535 // non-ABS limit

typedef struct
{
    float angle, spd, Kp, Kd, torque;
} J60_CTRL_t;

typedef struct
{
    float angle, spd, torque;
    uint8_t temp_flag;
    float temp;
} J60_FDBK_t;

typedef struct
{
    uint16_t OverVOLT : 1;
    uint16_t UnderVOLT : 1;
    uint16_t OverCURR : 1;
    uint16_t OverTEMP_motor : 1;
    uint16_t OverTEMP_driver : 1;
    uint16_t CAN_timeout : 1;
    uint16_t TEMPline_disconnect : 1;
    uint16_t reseverd : 9;
} J60_StatusWord_t;

typedef struct
{
    J60_CTRL_t CTRL;
    J60_FDBK_t FDBK;
    J60_StatusWord_t StatusWord;
} J60_t;


void J60_SendCMD(void *CAN_handle, uint8_t ID, uint16_t J60_CMD, float data);

#endif