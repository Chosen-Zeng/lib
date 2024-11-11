#include "TIM.h"

#ifndef TIMSW
#error Software TIM
#endif

void TIMSW_UpdateInterval(time_t *time_struct) // must < 1s
{
    time_struct->prev = time_struct->curr;
    time_struct->curr = TIMSW_TIME;

    if (time_struct->prev > time_struct->curr)
        time_struct->interval = 1 + time_struct->curr - time_struct->prev;
    else
        time_struct->interval = time_struct->curr - time_struct->prev;
}

uint8_t TIMSW_TimeLimit(time_t *time_struct, float time_limit)
{
    time_struct->prev = time_struct->curr;
    time_struct->curr = TIMSW_TIME;

    if (time_struct->prev > time_struct->curr) // interval as a second counter
        return ++time_struct->interval - time_struct->prev + time_struct->curr < time_limit;
    else
        return time_struct->interval + time_struct->curr - time_struct->prev < time_limit;
}