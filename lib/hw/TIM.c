#include "TIM.h"

#ifdef TIMSW
//  @brief     update interval in the specific time struct
//  @attention interval must < 1s
void TIMSW_UpdateInterval(time_t *time_struct)
{
    if (time_struct->prev != 0 && time_struct->curr != 0)
    {
        time_struct->prev = time_struct->curr;
        time_struct->curr = TIMSW_TIME;

        if (time_struct->prev > time_struct->curr)
            time_struct->interval = 1 + time_struct->curr - time_struct->prev;
        else
            time_struct->interval = time_struct->curr - time_struct->prev;
    }
    else
        time_struct->prev = time_struct->curr = TIMSW_TIME;
}

// @return time within limit or not
unsigned char TIMSW_TimeLimit(time_t *time_struct, float time_limit)
{
    if (time_struct->prev != 0 && time_struct->curr != 0)
    {
        time_struct->prev = time_struct->curr;
        time_struct->curr = TIMSW_TIME;

        if (time_struct->prev > time_struct->curr)
            time_struct->interval += 1 + time_struct->curr - time_struct->prev;
        else
            time_struct->interval += time_struct->curr - time_struct->prev;
    }
    else
        time_struct->prev = time_struct->curr = TIMSW_TIME;

    return time_struct->interval < time_limit;
}

void TIMSW_ClearTime(time_t *time_struct)
{
    time_struct->interval = time_struct->curr = time_struct->prev = 0;
}

float TIMSW_GetTimeRatio(time_t *time_struct, float period)
{
    return time_struct->interval > period ? 1 : time_struct->interval / period;
}
#endif