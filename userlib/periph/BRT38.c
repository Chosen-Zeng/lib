#include "stm32g4xx_hal.h"
#include "cmsis_os.h"
#include "FDCAN.h"
#include "BRT38.h"
#include "BRT38_INST.h"

#define ABS(X) ((X) >= 0 ? (X) : -(X)) // 输出X绝对值

extern DMA_HandleTypeDef hdma_memtomem_dma1_channel2;

struct BRT38
{
    uint8_t length;
    uint8_t addr;
    uint8_t func;
    uint8_t data[4];
} BRT38_CTRL;
float BRT38_angle;

#ifdef DEBUG
uint8_t BRT38_status;
#endif

void BRT38_SendData(FDCAN_HandleTypeDef *hfdcan, uint8_t addr, uint8_t func, uint32_t data, uint8_t datalength);

void BRT38_Init(FDCAN_HandleTypeDef *hfdcan, uint8_t addr)
{
    BRT38_SendData(hfdcan, addr, BRT38_ATD_SET, 1000, 2);
    vTaskDelay(1);
    BRT38_SendData(hfdcan, addr, BRT38_MODE_SET, BRT38_MODE_VAL, 1);
    vTaskDelay(1);
    BRT38_SendData(hfdcan, addr, BRT38_INC_DIRCT_SET, BRT38_INC_DIRCT_CCW, 1);
    vTaskDelay(1);
    BRT38_SendData(hfdcan, addr, BRT38_ZP_SET, 0, 1);
}

void BRT38_SendData(FDCAN_HandleTypeDef *hfdcan, uint8_t addr, uint8_t func, uint32_t data, uint8_t datalength)
{
    BRT38_CTRL.length = datalength + 3;
    BRT38_CTRL.addr = addr;
    BRT38_CTRL.func = func;
    *(uint32_t *)BRT38_CTRL.data = data;

    FDCAN_SendData(hfdcan, BRT38_CTRL.addr, &BRT38_CTRL, BRT38_CTRL.length);
}

__weak void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    uint8_t RxFifo0[7];
    if (hfdcan->Instance == FDCAN1)
    {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

        static struct BRT38 BRT38_FDBK;

        HAL_DMA_Start(&hdma_memtomem_dma1_channel2, (uint32_t)RxFifo0, (uint32_t)&BRT38_FDBK, RxFifo0[0]);
        HAL_DMA_PollForTransfer(&hdma_memtomem_dma1_channel2, HAL_DMA_FULL_TRANSFER, 1);

        static int16_t BRT38_lap_sgl, BRT38_lap_mpl;
        static float BRT38_angle_sgl, BRT38_angle_mpl;

        switch (BRT38_FDBK.func)
        {
        case BRT38_VAL_READ:
        {
            static float angle_prev;
#ifdef BRT38_LAP_SGL

            BRT38_angle_sgl = *(uint32_t *)BRT38_FDBK.data * BRT38_fANGLE;

            if (ABS(BRT38_angle_sgl - angle_prev) > 360 * 0.9)
                BRT38_angle_sgl > angle_prev ? BRT38_lap_sgl-- : BRT38_lap_sgl++;
            angle_prev = BRT38_angle_sgl;

            BRT38_angle = BRT38_lap_mpl * 360 + BRT38_angle_mpl;
#endif
#ifdef BRT38_LAP_MPL

            BRT38_angle_mpl = *(uint32_t *)BRT38_FDBK.data * BRT38_fANGLE;

            if (ABS(BRT38_angle_mpl - angle_prev) > BRT38_LAP * 360 * 0.9)
                BRT38_angle_mpl > angle_prev ? BRT38_lap_mpl-- : BRT38_lap_mpl++;
            angle_prev = BRT38_angle_mpl;

            BRT38_angle = BRT38_lap_mpl * BRT38_LAP * 360 + BRT38_angle_mpl;
#endif
            break;
        }
#ifdef DEBUG
        default:
        {
            if (BRT38_FDBK.data[0])
                BRT38_status = BRT38_ERROR;
            else
                BRT38_status = BRT38_SUCCESS;
        }
#endif
        }
    }
}