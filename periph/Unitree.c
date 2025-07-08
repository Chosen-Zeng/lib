#include "Unitree.h"
#include "USART.h"
#include "algorithm.h"
#include "CRC.h"

#ifdef GO_M8010_6_NUM

void GO_M8010_6_SendParam(USART_handle_t *const UART_infoconst, const unsigned char arrID)
{
    *(unsigned short *)GO_M8010_6[arrID].TxData = GO_M8010_6_PREAMBLE_SEND;
    GO_M8010_6[arrID].TxData[2] = GO_M8010_6[arrID].ID | GO_M8010_6_MODE_FOC << 4;
    *(short *)&GO_M8010_6[arrID].TxData[3] = LIMIT_ABS(GO_M8010_6[arrID].ctrl.trq, GO_M8010_6_TRQ_LIMIT) * GO_M8010_6_fTRQ;
    *(short *)&GO_M8010_6[arrID].TxData[5] = LIMIT_ABS(GO_M8010_6[arrID].ctrl.spd, GO_M8010_6_SPD_LIMIT) * GO_M8010_6_fSPD;
    *(int *)&GO_M8010_6[arrID].TxData[7] = LIMIT_ABS(GO_M8010_6[arrID].ctrl.pos, GO_M8010_6_POS_LIMIT) * GO_M8010_6_fPOS;
    *(unsigned short *)&GO_M8010_6[arrID].TxData[11] = LIMIT(GO_M8010_6[arrID].ctrl.Kp, GO_M8010_6_Kp_LIMIT) * GO_M8010_6_fKp;
    *(unsigned short *)&GO_M8010_6[arrID].TxData[13] = LIMIT(GO_M8010_6[arrID].ctrl.Kd, GO_M8010_6_Kd_LIMIT) * GO_M8010_6_fKd;
    *(unsigned short *)&GO_M8010_6[arrID].TxData[15] = CRCsw_Calc(&CRC_16_CCITT, GO_M8010_6[arrID].TxData, 15);

    UART_SendArray(UART_info, GO_M8010_6[arrID].TxData, 17);
}

void GO_M8010_6_Stop(USART_handle_t *const UART_info, const unsigned char arrID)
{
    *(unsigned short *)GO_M8010_6[arrID].TxData = GO_M8010_6_PREAMBLE_SEND;
    GO_M8010_6[arrID].TxData[2] = GO_M8010_6[arrID].ID | GO_M8010_6_MODE_STOP << 4;
    *(unsigned short *)&GO_M8010_6[arrID].TxData[15] = CRCsw_Calc(&CRC_16_CCITT, GO_M8010_6[arrID].TxData, 15);

    UART_SendArray(UART_info, GO_M8010_6[arrID].TxData, 17);
}

bool GO_M8010_6_MsgHandler(const unsigned char arrID, const unsigned char RxData[16])
{
    if (*(unsigned short *)RxData == GO_M8010_6_PREAMBLE_RECV &&
        GO_M8010_6[arrID].ID == (RxData[2] & 0xF) &&
        !CRC_16_Cal(&CRC_16_CCITT, RxData, 16))
    {
        GO_M8010_6[arrID].fdbk.mode = (RxData[2] & 0x70) >> 4;
        GO_M8010_6[arrID].fdbk.trq = *(short *)&RxData[3] / GO_M8010_6_fTRQ;
        GO_M8010_6[arrID].fdbk.spd = *(short *)&RxData[5] / GO_M8010_6_fSPD;
        GO_M8010_6[arrID].fdbk.pos = *(int *)&RxData[7] / GO_M8010_6_fPOS;
        GO_M8010_6[arrID].fdbk.temp = RxData[11];
        GO_M8010_6[arrID].fdbk.err_motor = RxData[12] & 0x7;
        GO_M8010_6[arrID].fdbk.footforce = (*(unsigned short *)&RxData[12] & 0xEFF8) >> 3;

        return true;
    }
    return false;
}
#endif

#ifdef A1_NUM

void A1_SendParam(USART_handle_t *const UART_info, const unsigned char arrID)
{
    *(unsigned short *)A1[arrID].TxData = A1_PREAMBLE;
    A1[arrID].TxData[2] = A1[arrID].ID;
    A1[arrID].TxData[4] = A1_MODE_FOC;
    A1[arrID].TxData[5] = 0xFF;
    *(short *)&A1[arrID].TxData[12] = LIMIT_ABS(A1[arrID].ctrl.trq, A1_TRQ_LIMIT) * A1_fTRQ;
    *(short *)&A1[arrID].TxData[14] = LIMIT_ABS(A1[arrID].ctrl.spd, A1_SPD_LIMIT) * A1_fSPD_MOTOR;
    *(int *)&A1[arrID].TxData[16] = LIMIT_ABS(A1[arrID].ctrl.pos, A1_POS_LIMIT) * A1_fPOS_MOTOR;
    *(unsigned short *)&A1[arrID].TxData[20] = LIMIT(A1[arrID].ctrl.Kp, A1_Kp_LIMIT) * A1_fKp;
    *(unsigned short *)&A1[arrID].TxData[22] = LIMIT(A1[arrID].ctrl.Kd, A1_Kd_LIMIT) * A1_fKd;
    *(unsigned *)&A1[arrID].TxData[30] = CRC_Calc(A1[arrID].TxData, 28, CRC_DATA_WORD);

    UART_SendArray(UART_info, A1[arrID].TxData, 34);
}

void A1_Stop(USART_handle_t *const UART_info, const unsigned char arrID)
{
    *(unsigned short *)A1[arrID].TxData = A1_PREAMBLE;
    A1[arrID].TxData[2] = A1[arrID].ID;
    A1[arrID].TxData[4] = A1_MODE_STOP;
    A1[arrID].TxData[5] = 0xFF;
    *(unsigned *)&A1[arrID].TxData[30] = CRC_Calc(A1[arrID].TxData, 28, CRC_DATA_WORD);

    UART_SendArray(UART_info, A1[arrID].TxData, 34);
}

bool A1_MsgHandler(const unsigned char arrID, const unsigned char RxData[78])
{
    if (*(unsigned short *)RxData == A1_PREAMBLE &&
        A1[arrID].ID == RxData[2] &&
        CRC_Calc(RxData, 72, CRC_DATA_WORD) == *(unsigned *)&RxData[74])
    {
        A1[arrID].fdbk.temp.motor = RxData[6];
        A1[arrID].fdbk.err.motor = RxData[7];
        A1[arrID].fdbk.trq = *(short *)&RxData[12] / A1_fTRQ;
        A1[arrID].fdbk.spd.motor = *(short *)&RxData[14] / A1_fSPD_MOTOR;
        A1[arrID].fdbk.spd.motor_fltr = *(float *)&RxData[16];
        A1[arrID].fdbk.spd.enc = *(short *)&RxData[20] / A1_fSPD_ENC;
        A1[arrID].fdbk.spd.enc_fltr = *(float *)&RxData[22];
        A1[arrID].fdbk.acc.motor = *(short *)&RxData[26] / A1_GR;
        A1[arrID].fdbk.acc.enc = *(short *)&RxData[28] / A1_fACC_ENC;
        A1[arrID].fdbk.pos.motor = *(int *)&RxData[30] / A1_fPOS_MOTOR;
        A1[arrID].fdbk.pos.enc = *(int *)&RxData[34];
        A1[arrID].fdbk.IMU.board.gyro.x = *(short *)&RxData[38] / A1_fGYRO;
        A1[arrID].fdbk.IMU.board.gyro.y = *(short *)&RxData[40] / A1_fGYRO;
        A1[arrID].fdbk.IMU.board.gyro.z = *(short *)&RxData[42] / A1_fGYRO;
        A1[arrID].fdbk.IMU.board.acc.x = *(short *)&RxData[44] / A1_fACC;
        A1[arrID].fdbk.IMU.board.acc.y = *(short *)&RxData[46] / A1_fACC;
        A1[arrID].fdbk.IMU.board.acc.z = *(short *)&RxData[48] / A1_fACC;
        A1[arrID].fdbk.IMU.foot.gyro.x = *(short *)&RxData[50];
        A1[arrID].fdbk.IMU.foot.gyro.y = *(short *)&RxData[52];
        A1[arrID].fdbk.IMU.foot.gyro.z = *(short *)&RxData[54];
        A1[arrID].fdbk.IMU.foot.acc.x = *(short *)&RxData[56];
        A1[arrID].fdbk.IMU.foot.acc.y = *(short *)&RxData[58];
        A1[arrID].fdbk.IMU.foot.acc.z = *(short *)&RxData[60];
        A1[arrID].fdbk.IMU.foot.mag.x = *(short *)&RxData[62];
        A1[arrID].fdbk.IMU.foot.mag.y = *(short *)&RxData[64];
        A1[arrID].fdbk.IMU.foot.mag.z = *(short *)&RxData[66];
        A1[arrID].fdbk.temp.foot = RxData[68] * A1_fTEMP_FOOT + A1_TEMP_FOOT_OFFSET;
        A1[arrID].fdbk.footforce = *(unsigned short *)&RxData[69] << 16 | RxData[71];
        A1[arrID].fdbk.err.foot = RxData[72];

        return true;
    }
    return false;
}
#endif

#if !defined GO_M8010_6_NUM && !defined A1_NUM
#error GO_M8010_6_NUM && A1_NUM undefined
#endif