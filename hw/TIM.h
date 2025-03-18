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

//  @brief     update intvl of the specific time struct
//  @attention intvl must < 1s
static inline float Timer_GetInterval(timer_t *timer_struct)
{
    if (timer_struct->prev != 0 && timer_struct->curr != 0)
    {
        timer_struct->prev = timer_struct->curr;
        timer_struct->curr = TIMER_TIME;

        if (timer_struct->prev > timer_struct->curr)
            timer_struct->intvl = 1 + timer_struct->curr - timer_struct->prev;
        else
            timer_struct->intvl = timer_struct->curr - timer_struct->prev;
    }
    else
        timer_struct->prev = timer_struct->curr = TIMER_TIME;

    return timer_struct->intvl;
}

//  @brief     update elapse of the specific time struct
//  @attention intvl must < 1s
static inline float Timer_GetElapse(timer_t *timer_struct)
{
    if (timer_struct->prev != 0 && timer_struct->curr != 0)
    {
        timer_struct->prev = timer_struct->curr;
        timer_struct->curr = TIMER_TIME;

        if (timer_struct->prev > timer_struct->curr)
            timer_struct->intvl += 1 + timer_struct->curr - timer_struct->prev;
        else
            timer_struct->intvl += timer_struct->curr - timer_struct->prev;
    }
    else
        timer_struct->prev = timer_struct->curr = TIMER_TIME;

    return timer_struct->intvl;
}

// @return timeout or not
static inline unsigned char Timer_CheckTimeout(timer_t *timer_struct, float timeout)
{
    Timer_GetElapse(timer_struct);
    return timer_struct->intvl >= timeout;
}

static inline void Timer_Clear(timer_t *timer_struct)
{
    timer_struct->intvl = timer_struct->curr = timer_struct->prev = 0;
}

static inline float Timer_GetTimeRatio(timer_t *timer_struct, float duration)
{
    return Timer_GetElapse(timer_struct) > duration ? 1 : timer_struct->intvl / duration;
}

#undef TIMER_TIME
#else
#error No TIM for timer defined.
#endif
#endif