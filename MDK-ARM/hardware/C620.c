#include "FDCAN.h"
#include "stm32g4xx_hal.h"
#include "C620.h"

#include <string.h>
#include <stdio.h>

#define ABS(X) ((X) >= 0? (X): -(X))
#define ABS_LIMIT(X, Y) (X >= 0? (X = Y): (X = -Y))

//#define ANGLE_Kp 1
//#define ANGLE_Ki 0
//#define ANGLE_Kd 0
float ANGLE_Kp = 1, ANGLE_Ki = 0, ANGLE_Kd = 0;

//#define ANGLE_pDEADBAND 0
//#define ANGLE_iSTART 0
//#define ANGLE_iLIMIT 0
int ANGLE_pDEADBAND = 26, ANGLE_iSTART = 200, ANGLE_iLIMIT = 10;

#define RPM_Kp 2.4e-2
#define RPM_Ki 1e-2
#define RPM_Kd 3.8e-5

#define RPM_pDEADBAND 26
#define RPM_iSTART 275
#define RPM_iLIMIT 25

extern FDCAN_HandleTypeDef hfdcan1;
extern DMA_HandleTypeDef hdma_memtomem_dma1_channel2;


void C620_SetI(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);

struct C620
{
	float Angle[8];
	float RPM[8];
	float I[8];
	float Temp[8];
} C620_CTRL, C620_FDBK;

struct PID
{
	float p[8];
	float i[8];
	float d[8];
	float pterm[8];
	float iterm[8];
	float dterm[8];
	int16_t deprev[8];
	int16_t decurr[8];
} RPM, Angle;

static uint8_t C620[8];


//void C620_PID_Angle(FDCAN_HandleTypeDef* hfdcan, uint32_t ID)
//{
//	
//	for (int count = 0; count < 8; count++)
//	{

//		Angle.pterm[count] = C620_CTRL.Angle[count] - C620_FDBK.Angle[count];
//		
////		if (ABS(Angle.pterm[count]) >= ANGLE_iSTART)	//积分分离
////		{
////			Angle.iterm[count] = 0;
////		}
////		else if (ABS(Angle.iterm[count]) <= ANGLE_iLIMIT && ABS(Angle.pterm[count]) >= ANGLE_iSTOP)	//积分限幅 & 死区
////		{
////			Angle.iterm[count] += Angle.pterm[count] * time;
////			if (ABS(Angle.iterm[count]) > ANGLE_iLIMIT)
////				ABS_LIMIT(Angle.iterm[count], ANGLE_iLIMIT);
////		}

////		Angle.dterm[count] = Angle.decurr[count] - Angle.deprev[count];	//微分先行
//		

//		C620_CTRL.RPM[count] = ANGLE_Kp * Angle.pterm[count] + ANGLE_Ki * Angle.iterm[count] + ANGLE_Kd * Angle.dterm[count];

//	}
//	
//	C620_PID_RPM(hfdcan, ID);
//}

void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID)
{
	static float time;
	time = TIM6->CNT / 4000.f;
	TIM6->CNT = 0;
	
	for (int count = 0; count < 8; count++)
	{

		if (ABS(RPM.pterm[count] = C620_CTRL.RPM[count] - C620_FDBK.RPM[count]) < RPM_pDEADBAND)	//死区
			RPM.pterm[count] = 0;
		RPM.p[count] = RPM.pterm[count] * RPM_Kp;

		if (ABS(RPM.pterm[count]) >= RPM_iSTART)	//积分分离
		{
			RPM.iterm[count] = 0;
		}
		else if (ABS(RPM.iterm[count]) <= RPM_iLIMIT)	//积分限幅
		{
			RPM.iterm[count] += RPM.pterm[count] * time;
			if (ABS(RPM.iterm[count]) > RPM_iLIMIT)
				ABS_LIMIT(RPM.iterm[count], RPM_iLIMIT);
		}
		RPM.i[count] = RPM.iterm[count] * RPM_Ki;

		RPM.dterm[count] = (RPM.decurr[count] - RPM.deprev[count]) / time;	//微分先行
		RPM.d[count] = RPM.dterm[count] * RPM_Kd;

		C620_CTRL.I[count] = RPM.p[count] + RPM.i[count] + RPM.d[count];

	}
	
	C620_SetI(hfdcan, ID);
}

void C620_SetI(FDCAN_HandleTypeDef* hfdcan, uint32_t ID)
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