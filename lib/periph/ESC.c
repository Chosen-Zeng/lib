#include "ESC.h"
#include "algorithm.h"

// @note require 1 : 1us of ARR 
void ESC_SetCurrent(volatile unsigned int *TIM_CCR, ESC_prop_t *ESC, float *curr)
{
    LIMIT(*curr, ESC->CURR_MAX)
    *TIM_CCR = *curr * (ESC->ACCELERATOR_us_MAX - ESC->ACCELERATOR_us_MIN) / ESC->CURR_MAX + ESC->ACCELERATOR_us_MIN;
}