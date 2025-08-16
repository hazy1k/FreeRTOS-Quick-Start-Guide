/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "key.h"
#include "led.h"
#include "lcd.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* LCDˢ��ʱʹ�õ���ɫ */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/*----------------����������-----------------*/
QueueHandle_t xQueue; // ���о��
#define QUEUE_LENGTH 1 // ���г���
#define QUEUE_ITEM_SIZE sizeof(uint8_t) // ������ÿ����Ϣ�Ĵ�С
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
    lcd_show_string(10, 47, 220, 24, 24, "Message Queue", RED);
    lcd_draw_rectangle(5,130,234,314,BLACK);
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
    // ��������
    xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
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
    uint8_t key_value =0;
    while(1)
    {
        key_value = key_scan(0);
        if(key_value != 0)
        {
            xQueueSend(xQueue, &key_value, portMAX_DELAY); // �������ݵ�����
        }
        vTaskDelay(10);
    }
}

// task2����ʵ��
void task2(void *pvParameters)
{
    uint8_t queue_recv = 0; // ���ն�������
    uint32_t task2_num = 0; // ����2���д���
    while(1)
    {
        xQueueReceive(xQueue, &queue_recv, portMAX_DELAY); // ���ն�������
        switch(queue_recv)
        {
            case KEY0_PRES:
            {
                lcd_fill(6,131,233,313,lcd_discolor[++task2_num%11]);
                break;
            }
            case KEY1_PRES:
            {
                LED0_TOGGLE();
                break;
            }
            default:break;
        }
    }
}
/*---------------------------------------------*/
