#ifndef __UNITREE_H
#define __UNITREE_H

#include "usr.h"

#ifdef GO_M8010_6_NUM

#define GO_M8010_6_GR (19 / 3.f)

#define GO_M8010_6_PREAMBLE_SEND 0xEEFE
#define GO_M8010_6_PREAMBLE_RECV 0xEEFD

#define GO_M8010_6_MODE_STOP 0
#define GO_M8010_6_MODE_FOC 1
#define GO_M8010_6_MODE_CAL 2

#define GO_M8010_6_fTRQ 255
#define GO_M8010_6_fSPD (255 / R2D * GO_M8010_6_GR)
#define GO_M8010_6_fPOS (32767 / R2D * GO_M8010_6_GR)
#define GO_M8010_6_fKp 1279
#define GO_M8010_6_fKd 1279

#define GO_M8010_6_TRQ_LIMIT 128
#define GO_M8010_6_SPD_LIMIT (128 * R2D / GO_M8010_6_GR)
#define GO_M8010_6_POS_LIMIT (65536 * R2D / GO_M8010_6_GR)
#define GO_M8010_6_Kp_LIMIT 25.6f
#define GO_M8010_6_Kd_LIMIT 25.6f

enum GO_M8010_6_ERR
{
    NONE,
    OH,
    OC,
    OV,
    ENC_FAULT,
    BUSBAR_UV,
    WINDING_OH
};

typedef struct
{
    const unsigned char ID;

    struct
    {
        float trq, spd, pos, Kp, Kd;
    } ctrl;

    struct
    {
        float trq, spd, pos;
        char temp;
        enum GO_M8010_6_ERR : 3;
        unsigned shortfootforce : 12;
    } fdbk;
    unsigned char TxData[17];
} GO_M8010_6_t;
extern GO_M8010_6_t GO_M8010_6[GO_M8010_6_NUM];

void GO_M8010_6_SendParam(USART_handle_t *const USART_handle, const unsigned char arrID);
void GO_M8010_6_Stop(USART_handle_t *const UART_info, const unsigned char arrID);

bool GO_M8010_6_MsgHandler(const unsigned char arrID, const unsigned char RxData[17]);

#endif

#ifdef A1_NUM

#define A1_GR 9.1f

#define A1_PREAMBLE 0xEEFE

#define A1_ID_BCAST 0xBB
#define A1_BCAST_ARRID A1_NUM

#define A1_MODE_STOP 0
#define A1_MODE_SPIN 5
#define A1_MODE_FOC 10

#define A1_fTRQ 255.f
#define A1_fSPD_MOTOR (127 / R2D * A1_GR)
#define A1_fPOS_MOTOR (16383 / 360.f * A1_GR)
#define A1_fSPD_ENC (127 / R2D)
#define A1_fPOS_ENC (16383 / 360.f)
#define A1_fKp 2047.f
#define A1_fKd 1023.f
#define A1_fACC_ENC 8.f
#define A1_fGYRO (2000.f / 32768)
#define A1_fACC (8.f / 32768)
#define A1_fTEMP_FOOT 0.5f
#define A1_TEMP_FOOT_OFFSET -28

#define A1_TRQ_LIMIT 128
#define A1_SPD_LIMIT (256 * R2D)
#define A1_POS_LIMIT (131072 * 360)
#define A1_Kp_LIMIT 16
#define A1_Kd_LIMIT 32

typedef struct
{
    const unsigned char ID;
    struct
    {
        float trq, spd, pos, Kp, Kd;
    } ctrl;
    struct
    {
        struct
        {
            char motor, foot;
        } err;
        float trq;
        struct
        {
            float motor, motor_fltr, enc, enc_fltr;
        } spd;
        struct
        {
            float motor, enc;
        } acc, pos;
        struct
        {
            struct
            {
                struct
                {
                    float x, y, z;
                } gyro, acc;
            } board;
            struct
            {
                struct
                {
                    float x, y, z;
                } gyro, acc, mag;
            } foot;
        } IMU;
        struct
        {
            char motor;
            float foot;
        } temp;
        unsigned footforce : 24;
    } fdbk;
    unsigned char TxData[34];
} A1_t;
extern A1_t A1[A1_NUM + 1];

void A1_SendParam(USART_handle_t *const UART_info, const unsigned char arrID);
void A1_Stop(USART_handle_t *const UART_info, const unsigned char arrID);

bool A1_MsgHandler(const unsigned char arrID, const unsigned char RxData[78]);

#endif
#endif