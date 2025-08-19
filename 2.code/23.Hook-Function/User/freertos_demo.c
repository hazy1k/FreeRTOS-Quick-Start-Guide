/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

/*----------------����������-----------------*/
// START_TASK����
#define START_TASK_PRIO 1           // �������ȼ�
#define START_STK_SIZE  128         // �����ջ��С
TaskHandle_t StartTask_Handler;     // ������
void start_task(void *pvParameters);// ������

// TASK1����
#define TASK1_PRIO      2                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);
/*---------------------------------------------*/

/*------------------����ʵ����------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"HOOK",RED);    
    delay_ms(2000);  
    // ����START_TASK����
    xTaskCreate((TaskFunction_t)start_task,        // ������
                (const char*)"start_task",         // ��������
                (uint16_t)START_STK_SIZE,          // �����ջ��С
                (void*)NULL,                       // ���ݸ��������Ĳ���
                (UBaseType_t)START_TASK_PRIO,      // �������ȼ�
                (TaskHandle_t*)&StartTask_Handler);// ������
    // ��ʼ�������
    vTaskStartScheduler();
}

// start_task����ʵ��
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); // �����ٽ���
    lcd_display_off();
    LCD_BL(0);
    // ����TASK1����
    xTaskCreate((TaskFunction_t)task1,  
                (const char*)"task1",       
                (uint16_t)TASK1_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK1_PRIO,    
                (TaskHandle_t*)&Task1Task_Handler);
    vTaskDelete(StartTask_Handler); // ��ʼ�����Ѿ�����Լ�ʹ����ɾ���Լ�
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

void task1(void *pvParameters)
{
    while(1)
    {
        LED0(1); // LED0�𣬴����˳��͹���ģʽ
        delay_ms(3000); // ģ���������У���ʱ�������͹���ģʽ
        LED0(0); // LED0�����������͹���ģʽ
        vTaskDelay(3000);
    }
}

void before_sleep(void)
{
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
}

void after_wakeup(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
}

// ���������Ӻ���
void vApplicationIdleHook(void)
{
    __disable_irq();
    __dsb(0);
    __isb(0);
    before_sleep();

    // ����˯��ģʽ���ȴ��жϻ��ѣ�
    __WFI();

    after_wakeup();
    __dsb(0);
    __isb(0);
    __enable_irq();
}
/*---------------------------------------------*/