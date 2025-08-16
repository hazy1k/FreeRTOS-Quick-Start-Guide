/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "key.h"
#include "led.h"
#include "lcd.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*----------------����������-----------------*/
static QueueSetHandle_t xQueueSet; // ���м�
QueueHandle_t xQueuel; // ����1
SemaphoreHandle_t xSemaphore; // ��ֵ�ź���
#define QUEUE_LENGTH    1 // ���г���
#define QUEUE_ITEM_SIZE sizeof(uint32_t) // ÿ����Ϣ��С
#define SEMAPHORE_BINARY_LENGTH  1 // ��ֵ�ź�����Ч����
#define QUEUESET_LENGTH (QUEUE_LENGTH+SEMAPHORE_BINARY_LENGTH) // ���м�����
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
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);
/*---------------------------------------------*/

/*------------------����ʵ����------------------*/
void freertos_demo(void)
{
    lcd_show_string(10, 47, 220, 24, 24, "Queue Set", RED);
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
    // �������м�
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    // ��������1
    xQueuel = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    // ������ֵ�ź���
    xSemaphore = xSemaphoreCreateBinary();
    // ������1�Ͷ�ֵ�ź���������м�
    xQueueAddToSet(xQueuel, xQueueSet);
    xQueueAddToSet(xSemaphore, xQueueSet);
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
    vTaskDelete(StartTask_Handler); // ��ʼ�����Ѿ�����Լ�ʹ����ɾ���Լ�
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// task1����ʵ��
void task1(void *pvParameters)
{
    uint32_t key_value = 0;  // �ĳ� uint32_t
    while(1)
    {
        uint8_t temp = key_scan(0);  // ��ʱ�� uint8_t ����
        switch(temp)
        {
            case KEY0_PRES:
                key_value = temp;  // ��ֵ�� 32 λ����
                xQueueSend(xQueuel, &key_value, portMAX_DELAY);
                break;
            case KEY1_PRES:
                xSemaphoreGive(xSemaphore);
                break;
            default:
                break;
        }
        vTaskDelay(10);
    }
}

// task2����ʵ��
void task2(void *pvParameters)
{
    QueueSetMemberHandle_t activate_member = NULL;
    uint32_t queue_recv = 0;
    while(1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY); // �ȴ����м��еĶ��н��ܵ���Ϣ
        if(activate_member == xQueuel)
        {
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
					printf("recv from queue1:%d\r\n", queue_recv);
        }
        else if(activate_member == xSemaphore)
        {
            xSemaphoreTake(activate_member, portMAX_DELAY); // �ȴ���ֵ�ź����ͷ�
            printf("recv from semaphore\r\n");
        }
    }
}
/*---------------------------------------------*/
