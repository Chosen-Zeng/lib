#include "TIM.h"

#ifdef TIMSW // @note define TIMSW TIMx
#define TIMSW_TIME ((float)(TIMSW->CNT + 1) / (TIMSW->ARR + 1))

//  @brief     update interval in the specific time struct
//  @attention interval must < 1s
inline void TIMSW_UpdateInterval(timer_t *time_struct)
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
inline unsigned char TIMSW_TimeLimit(timer_t *time_struct, float time_limit)
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

inline void TIMSW_ClearTime(timer_t *time_struct)
{
    time_struct->interval = time_struct->curr = time_struct->prev = 0;
}

inline float TIMSW_GetTimeRatio(timer_t *time_struct, float period)
{
    return time_struct->interval > period ? 1 : time_struct->interval / period;
}

#else
#error No TIM for swTIM defined.
#endif