# User Manual
## userlib
### hardware
#### FDCAN
- Exported Function:
```C
void FDCAN_SendData(FDCAN_HandleTypeDef* hfdcan, uint32_t ID, uint8_t TxData[]);
```
### periph
#### C620
- Exported Function:
```C
void C620_PID_Angle(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
```
- Exported Macro
```C
#define ANGLE_MODE	//mode selection: RPM_MODE/ANGLE_MODE

#define C620_ID1 0x200
#define C620_ID2 0x1FF
```
- Exported Variable
```C
struct C620
{
	float Angle[8];
	float RPM[8];
	float I[8];
	float Temp[8];
};
```