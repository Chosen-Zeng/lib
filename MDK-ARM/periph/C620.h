#ifndef __C620_H
#define __C620_H

#define fAngle (360 / 8192.F)
#define fI (20 / 16384.F)


void C620_PID_Angle(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);

#endif