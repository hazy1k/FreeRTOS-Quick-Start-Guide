#include "btim.h"
#include "usart.h"
#include <stdio.h>

TIM_HandleTypeDef g_tim3_handle; // ��ʱ��3���
TIM_HandleTypeDef g_tim5_handle; // ��ʱ��5���

/**
 * @brief       ������ʱ��TIM3��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��D2PPRE1��2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ120M, ���Զ�ʱ��ʱ�� = 240Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void btim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    g_tim3_handle.Instance = BTIM_TIM3_INT;                      
    g_tim3_handle.Init.Prescaler = psc;                         
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         
    g_tim3_handle.Init.Period = arr;                            
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  
    HAL_TIM_Base_Init(&g_tim3_handle);
    HAL_TIM_Base_Start_IT(&g_tim3_handle); // ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����ж�
}

/**
 * @brief       ������ʱ��TIM5��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��D2PPRE1��2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ120M, ���Զ�ʱ��ʱ�� = 240Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
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
        HAL_NVIC_SetPriority(BTIM_TIM3_INT_IRQn, 4, 0); // �����ж����ȼ�����ռ���ȼ�4�������ȼ�0(����FreeRTOSӰ��)
        HAL_NVIC_EnableIRQ(BTIM_TIM3_INT_IRQn);          
    }
    else if (htim->Instance == BTIM_TIM5_INT)
    {
        BTIM_TIM5_INT_CLK_ENABLE();                      
        HAL_NVIC_SetPriority(BTIM_TIM5_INT_IRQn, 6, 0); // �����ж����ȼ�����ռ���ȼ�6�������ȼ�0(��FreeRTOSӰ��)
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
        printf("TIM3���\r\n");
    }
    else if (htim == (&g_tim5_handle))
    {
        printf("TIM5���\r\n");
    }
}
