/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "btim.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

/*----------------������������-----------------*/
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

/*----------------������ʵ����----------------*/
void freertos_demo(void)
{
    lcd_show_string(10,10,260,32,32,"FreeRTOS INT TEST",BLUE);
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
    btim_tim3_int_init(10000-1, 24000-1);
    btim_tim5_int_init(10000-1, 24000-1);
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

// task1����ʵ��
void task1(void *pvParameters)
{
    uint32_t task1_count = 0;
    while(1)
    {
        if(++task1_count == 5)
        {
            printf("FreeRTOS�ر�����Ӱ����жϣ�TIM3����Ӱ��");
            portDISABLE_INTERRUPTS(); // �ر��ж�
            LED0(0);
            delay_ms(5000);
            printf("FreeRTOS���´��жϣ�TIM3��������");
            LED0(1);
            portENABLE_INTERRUPTS(); // ���´��ж�
        }
        vTaskDelay(1000);
    }
}
/*---------------------------------------------*/
