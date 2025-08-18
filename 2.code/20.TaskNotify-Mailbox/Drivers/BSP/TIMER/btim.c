#include "btim.h"
#include "usart.h"
#include <stdio.h>

TIM_HandleTypeDef g_tim3_handle; // 定时器3句柄
TIM_HandleTypeDef g_tim5_handle; // 定时器5句柄

/**
 * @brief       基本定时器TIM3定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    g_tim3_handle.Instance = BTIM_TIM3_INT;                      
    g_tim3_handle.Init.Prescaler = psc;                         
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         
    g_tim3_handle.Init.Period = arr;                            
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  
    HAL_TIM_Base_Init(&g_tim3_handle);
    HAL_TIM_Base_Start_IT(&g_tim3_handle); // 使能定时器3和定时器3更新中断
}

/**
 * @brief       基本定时器TIM5定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_tim5_int_init(uint16_t arr, uint16_t psc)
{
    g_tim5_handle.Instance = BTIM_TIM5_INT;                   
    g_tim5_handle.Init.Prescaler = psc;                         
    g_tim5_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        
    g_tim5_handle.Init.Period = arr;                            
    g_tim5_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  
    HAL_TIM_Base_Init(&g_tim5_handle);
    HAL_TIM_Base_Start_IT(&g_tim5_handle); 
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIM3_INT)
    {
        BTIM_TIM3_INT_CLK_ENABLE();                     
        HAL_NVIC_SetPriority(BTIM_TIM3_INT_IRQn, 4, 0); // 设置中断优先级，抢占优先级4，子优先级0(不受FreeRTOS影响)
        HAL_NVIC_EnableIRQ(BTIM_TIM3_INT_IRQn);          
    }
    else if (htim->Instance == BTIM_TIM5_INT)
    {
        BTIM_TIM5_INT_CLK_ENABLE();                      
        HAL_NVIC_SetPriority(BTIM_TIM5_INT_IRQn, 6, 0); // 设置中断优先级，抢占优先级6，子优先级0(受FreeRTOS影响)
        HAL_NVIC_EnableIRQ(BTIM_TIM5_INT_IRQn);         
    }
}

void BTIM_TIM3_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim3_handle);
}

void BTIM_TIM5_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim5_handle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_tim3_handle))
    {
        printf("TIM3输出\r\n");
    }
    else if (htim == (&g_tim5_handle))
    {
        printf("TIM5输出\r\n");
    }
}
