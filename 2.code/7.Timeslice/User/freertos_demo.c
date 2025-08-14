/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include <stdio.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

/*-------------ȫ�ֱ�����������������------------*/
// LCD��ʾ��ɫ
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};
/*---------------------------------------------*/

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

// TASK2����
#define TASK2_PRIO      2 // ��ͬ�������ȼ�                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);
/*-------------------------------------------*/

/*------------------����ʵ����----------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Demo",RED);
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
    // ����TASK1����
    xTaskCreate((TaskFunction_t)task1,  
                (const char*)"task1",       
                (uint16_t)TASK1_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK1_PRIO,    
                (TaskHandle_t*)&Task1Task_Handler);
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); // ��ʼ�����Ѿ�����Լ�ʹ����ɾ���Լ�
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// task1����ʵ��
void task1(void *pvParameters)
{
    uint32_t task1_num = 0;
    while(1)
    {
        taskENTER_CRITICAL(); // �����ٽ���
        printf("����1���д�����%d\r\n",++task1_num);
        taskEXIT_CRITICAL(); // �˳��ٽ���
				vTaskDelay(10);
    }
}

// task2����ʵ��
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    while(1)
    {
        taskENTER_CRITICAL(); // �����ٽ���
        printf("����2���д�����%d\r\n",++task2_num);
        taskEXIT_CRITICAL(); // �˳��ٽ���
			  vTaskDelay(10);
    }
}

/*-------------------------------------------*/