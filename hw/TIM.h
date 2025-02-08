#ifndef __TIM_H
#define __TIM_H

#include "user.h"

#ifdef TIMER // define TIMER TIMx, TIM freq must be set 1Hz
typedef struct
{
    float intvl, curr, prev;
} timer_t;

#define timer_InitStruct {0, 0, 0}

#define TIMER_TIME ((float)(TIMER->CNT + 1) / (TIMER->ARR + 1))

//  @brief     update intvl in the specific time struct
//  @attention intvl must < 1s
static inline void Timer_GetInterval(timer_t *time_struct)
{
    if (time_struct->prev != 0 && time_struct->curr != 0)
    {
        time_struct->prev = time_struct->curr;
        time_struct->curr = TIMER_TIME;

        if (time_struct->prev > time_struct->curr)
            time_struct->intvl = 1 + time_struct->curr - time_struct->prev;
        else
            time_struct->intvl = time_struct->curr - time_struct->prev;
    }
    else
        time_struct->prev = time_struct->curr = TIMER_TIME;
}

// @return timeout or not
static inline unsigned char Timer_CheckTimeout(timer_t *time_struct, float timeout)
{
    if (time_struct->prev != 0 && time_struct->curr != 0)
    {
        time_struct->prev = time_struct->curr;
        time_struct->curr = TIMER_TIME;

        if (time_struct->prev > time_struct->curr)
            time_struct->intvl += 1 + time_struct->curr - time_struct->prev;
        else
            time_struct->intvl += time_struct->curr - time_struct->prev;
    }
    else
        time_struct->prev = time_struct->curr = TIMER_TIME;

    return time_struct->intvl >= timeout;
}

static inline void Timer_Clear(timer_t *time_struct)
{
    time_struct->intvl = time_struct->curr = time_struct->prev = 0;
}

static inline float Timer_GetTimeRatio(timer_t *time_struct, float duration)
{
    return time_struct->intvl > duration ? 1 : time_struct->intvl / duration;
}

#undef TIMER_TIME
#else
#error No TIM for timer defined.
#endif
#endif