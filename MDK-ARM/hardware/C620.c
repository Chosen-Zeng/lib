#include "FDCAN.h"
#include "stm32g4xx_hal.h"
#include "C620.h"

#include <string.h>
#include <stdio.h>

#define ABS(X) (X >= 0? X: -X)
#define ABS_LIMIT(X, Y) (X >= 0? (X = Y): (X = -Y))

#define TIME_ELPS 0.001	//1kHz

#define RPM_Kp (20 * TIME_ELPS)
#define RPM_Ki (20 * TIME_ELPS)
#define RPM_Kd 0.0016

#define RPM_iSTART 256
#define RPM_iSTOP 16
#define RPM_iLIMIT 12

extern FDCAN_HandleTypeDef hfdcan1;
extern DMA_HandleTypeDef hdma_memtomem_dma1_channel2;


void C620_SendData(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID)；

struct C620
{
	float Angle[8];
	float RPM[8];
	float I[8];
	float Temp[8];
} C620_CTRL, C620_FDBK;

struct PID
{
	float pterm[8];
	float iterm[8];
	float dterm[8];
	int16_t deprev[8];
	int16_t decurr[8];
} RPM;

static uint8_t C620[8];


void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID)
{
	
	for (int count = 0; count < 8; count++)
	{

		RPM.pterm[count] = C620_CTRL.RPM[count] - C620_FDBK.RPM[count];
		
		if (ABS(RPM.pterm[count]) >= RPM_iSTART)	//积分分离
		{
			RPM.iterm[count] = 0;
		}
		else if (ABS(RPM.iterm[count]) <= RPM_iLIMIT && ABS(RPM.pterm[count]) >= RPM_iSTOP)	//积分限幅 & 死区
		{
			RPM.iterm[count] += RPM.pterm[count] * TIME_ELPS;
			if (ABS(RPM.iterm[count]) > RPM_iLIMIT)
				ABS_LIMIT(RPM.iterm[count], RPM_iLIMIT);
		}

		RPM.dterm[count] = RPM.decurr[count] - RPM.deprev[count];	//微分先行
		

		C620_CTRL.I[count] = RPM_Kp * RPM.pterm[count] + RPM_Ki * RPM.iterm[count] + RPM_Kd * RPM.dterm[count];

	}
	
	C620_SendData(hfdcan, ID);
}

void C620_SendData(FDCAN_HandleTypeDef* hfdcan, uint32_t ID)
{
    for (int count = 0; count <= 3; count++)
    {
		if (ABS(C620_CTRL.I[count]) > 20)
			ABS_LIMIT(C620_CTRL.I[count], 20);
        C620[count * 2] = (int16_t)(C620_CTRL.I[count] / 20 * 16384) >> 8;
        C620[count * 2 + 1] = (int16_t)(C620_CTRL.I[count] / 20 * 16384);
    }

    FDCAN_SendData(hfdcan, ID, C620);
}