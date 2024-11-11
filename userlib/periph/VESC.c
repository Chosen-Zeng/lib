#include "main.h"
#include "VESC.h"
#include "VESC_INST.h"
#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

#define ABS(X) ((X) >= 0 ? (X) : -(X)) // 输出X绝对值
#define ABS_LIMIT(X, Y) \
    if (ABS(X) > Y)     \
    X >= 0 ? (X = Y) : (X = -Y)

VESC_FDBK_t VESC_FDBK[VESC_MAX];
float VESC_RPM[VESC_MAX], VESC_DutyCycle[VESC_MAX], VESC_Current[VESC_MAX];

void f2u8_4(const float *num, uint8_t TxData[4])
{
    for (int count = 0; count < 4; count++)
    {
        TxData[count] = (int32_t)*num >> 8 * (3 - count);
    }
}

void VESC_SendCMD(void *CAN_handle, uint8_t ID, uint8_t VESC_Command, float data)
{
    uint8_t TxData[4];

    switch (VESC_Command)
    {
    case VESC_SET_DUTY:
    {
        data *= VESC_fPCT_W;
        ABS_LIMIT(data, 1 * VESC_fPCT_W); // 1 or 100? That's a question. Can't understand the scaling of PCT.

        f2u8_4(&data, TxData);
        break;
    }
    case VESC_SET_CURRENT:
    {
        data *= VESC_fCURRENT_W;
        ABS_LIMIT(data, VESC_MOTOR_CURRENT_MAX * VESC_fCURRENT_W);

        f2u8_4(&data, TxData);
        break;
    }
    case VESC_SET_RPM:
    {
        data *= VESC_MOTOR_PP;
        ABS_LIMIT(data, VESC_MOTOR_RPM_MAX * VESC_MOTOR_PP);

        f2u8_4(&data, TxData);
        break;
    }
    }
    #ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_EXT, ID | VESC_Command << 8, TxData, 4);
    #elif defined FDCAN_SUPPORT
    FDCAN_SendData(CAN_handle, FDCAN_EXTENDED_ID, ID | VESC_Command << 8, TxData, 4);
    #endif
}

/*void HAL_FDCAN_RxFifo0Callback(void *CAN_handle, uint32_t RxFifo0ITs)
{
    uint8_t RxFifo0[8];
    if (hfdcan->Instance == FDCAN1)
    {
        FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

        switch ((FDCAN_RxHeader.Identifier & 0xFF00) >> 8)
        {
        case VESC_STATUS:
        {
            VESC_FDBK[FDCAN_RxHeader.Identifier & 0x00FF].RPM = (float)(RxFifo0[0] << 24 | RxFifo0[1] << 16 | RxFifo0[2] << 8 | RxFifo0[3]) / VESC_MOTOR_PP;
            break;
        }
        case VESC_STATUS_4:
        {
            VESC_FDBK[FDCAN_RxHeader.Identifier & 0x00FF].Current_In = (float)(RxFifo0[4] << 8 | RxFifo0[5]) / VESC_fCURRENT_R;
            break;
        }
        }
    }
}*/