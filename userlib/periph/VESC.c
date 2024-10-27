#include "stm32g4xx_hal.h"
#include "VESC.h"
#include "VESC_INST.h"
#include "FDCAN.h"

#define ABS(X) ((X) >= 0 ? (X) : -(X)) // 输出X绝对值
#define ABS_LIMIT(X, Y) \
    if (ABS(X) > Y)     \
    X >= 0 ? (X = Y) : (X = -Y)

float VESC_RPM[VESC_MAX], VESC_DutyCycle[VESC_MAX]; // VESC_Current[VESC_MAX],


void f2u8_4(const float *num, uint8_t TxData[4])
{
    for (int count = 0; count < 4; count++)
    {
        TxData[count] = (int32_t)*num >> 8 * (3 - count);
    }
}

void VESC_SendData(FDCAN_HandleTypeDef *hfdcan, uint8_t VESC_ID, uint8_t VESC_Command, float data)
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
    FDCAN_SendData(hfdcan, FDCAN_EXTENDED_ID, VESC_ID | VESC_Command << 8, TxData, 4);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    uint8_t RxData[8];
    if (hfdcan->Instance == FDCAN1)
    {
        FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxData);

        switch ((FDCAN_RxHeader.Identifier & 0xFF00) >> 8)
        {
        case VESC_STATUS:
        {
            VESC_RPM[FDCAN_RxHeader.Identifier & 0x00FF] = (float)(RxData[0] << 24 | RxData[1] << 16 | RxData[2] << 8 | RxData[3]) / VESC_MOTOR_PP;
            //            VESC_Current[FDCAN_RxHeader.Identifier & 0x00FF] = (float)(RxData[4] << 8 | RxData[5]) / VESC_fCURRENT_R;
            VESC_DutyCycle[FDCAN_RxHeader.Identifier & 0x00FF] = (float)(RxData[6] << 8 | RxData[7]) / VESC_fPCT_R;

            break;
        }
        }
    }
}