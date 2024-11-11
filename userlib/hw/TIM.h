#ifndef __TIM_H
#define __TIM_H

#include "main.h"

#define TIMSW_TIME ((float)(TIMSW->CNT + 1) / (TIMSW->ARR + 1))

#define time_InitStruct {0, 0, 0}

typedef struct
{
    float interval, curr, prev;
} time_t;

void TIMSW_UpdateInterval(time_t *time_struct); // must < 1s
uint8_t TIMSW_TimeLimit(time_t *time_struct, float time_limit);

#endif