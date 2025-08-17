/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* LCDˢ��ʱʹ�õ���ɫ */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/*----------------����������-----------------*/
// START_TASK����
#define START_TASK_PRIO 1           // �������ȼ� 
#define START_STK_SIZE  128         // �����ջ��С 
TaskHandle_t StartTask_Handler;     // ������ 
void start_task(void *pvParameters);// ������ 

// TASK1����
#define TASK1_PRIO      4                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);

// TASK2����
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3����
#define TASK3_PRIO      2                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);

SemaphoreHandle_t MutexSemaphore; // �����ź���
/*---------------------------------------------*/

/*------------------����ʵ����------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"MultiTask Demo",RED);
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
    // ���������ź���
    MutexSemaphore = xSemaphoreCreateMutex();  
    // ����TASK1����
    xTaskCreate((TaskFunction_t)task1,  
                (const char*)"task1",       
                (uint16_t)TASK1_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK1_PRIO,    
                (TaskHandle_t*)&Task1Task_Handler);
    // ����TASK2����
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    // ����TASK3����
    xTaskCreate((TaskFunction_t)task3,  
                (const char*)"task3",       
                (uint16_t)TASK3_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK3_PRIO,    
                (TaskHandle_t*)&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); // ��ʼ�����Ѿ�����Լ�ʹ����ɾ���Լ�
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// task1����ʵ��-���ȼ���
void task1(void *pvParameters)
{
    vTaskDelay(500);
    while(1)
    {
        printf("task1 ready to take mutex\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY); // ��ȡ�����ź���
        printf("task1 has taked mutex\r\n");
        printf("task1 running\r\n");
        printf("task1 give mutex\r\n");
        xSemaphoreGive(MutexSemaphore); // �ͷŻ����ź���
        vTaskDelay(100);

    }
}

// task2����ʵ��-���ȼ���
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    vTaskDelay(200);
    while(1)
    {
        for(task2_num = 0; task2_num < 5; task2_num++)
        {
            printf("task2 running\r\n");
            delay_ms(100); // ģ�����У��������������
        }
        vTaskDelay(1000);
    }
}

// task3����ʵ��-���ȼ���
void task3(void *pvParameters)
{
    uint32_t task3_num = 0;
    while(1)
    {
        printf("task3 ready to take mutex\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY); // ��ȡ�����ź���
        printf("task3 has taked mutex\r\n");
        for(task3_num = 0; task3_num < 5; task3_num++)
        {
            printf("task3 running\r\n");
            delay_ms(100); // ģ�����У��������������
        }
        printf("task3 give mutex\r\n");
        xSemaphoreGive(MutexSemaphore); // �ͷŻ����ź���
        vTaskDelay(1000);
    }
}
// �����������̣�task3��ȡ�����ź�������Ϊ���ȼ���ߣ�
// task3����5�κ��ͷŻ����ź���
// task2ͬʱ����5��
// task1��ȡ�����ź�������Ϊ���ȼ���ͣ�
/*---------------------------------------------*/
