// @note TIM freq must be set 1Hz
#ifndef __TIM_H
#define __TIM_H

#include "main.h"

#define time_InitStruct {0, 0, 0}

typedef struct
{
    float interval, curr, prev;
} timer_t;

void TIMSW_UpdateInterval(timer_t *time_struct);
unsigned char TIMSW_TimeLimit(timer_t *time_struct, float time_limit);
void TIMSW_ClearTime(timer_t *time_struct);
float TIMSW_GetTimeRatio(timer_t *time_struct, float period);

#endif