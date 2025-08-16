/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "key.h"
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
static QueueSetHandle_t xQueueSet; // ���м�
QueueHandle_t xQueue1; // ����1
QueueHandle_t xQueue2; // ����2
#define QUEUE_LENGTH 1 // ����֧����Ϣ����
#define QUEUE_ITEM_SIZE sizeof(uint32_t) // ������Ϣ��С
#define QUEUESET_LENGTH (2*QUEUE_LENGTH) // ���м�֧�ֶ��и���
#define EVENTBIT_0 (1<<0) // �¼�λ
#define EVENTBIT_1 (1<<1) 
#define EVENTBIT_ALL (EVENTBIT_0 | EVENTBIT_1)
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
    lcd_show_string(10, 47, 220, 24, 24, "Queue Event Group", RED);
    lcd_draw_rectangle(5, 130, 234, 314, BLACK);
    lcd_show_string(30, 110, 220, 16, 16, "Event Group Value: 0", BLUE);
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
    xQueue1 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    // ��������2
    xQueue2 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    // ������1��2������м�
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);
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
    uint32_t key_value =0;
    uint32_t eventbit_0 = EVENTBIT_0;
    uint32_t eventbit_1 = EVENTBIT_1;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xQueueSend(xQueue1, &eventbit_0, portMAX_DELAY); // �����¼�������1
                break;
            }
            case KEY1_PRES:
            {
                xQueueSend(xQueue2, &eventbit_1, portMAX_DELAY);
                break;
            }
            default:break;
        }
        vTaskDelay(10); 
    }
}

// task2����ʵ��
void task2(void *pvParameters)
{
    uint32_t event_val = 0;
    uint32_t event_recv = 0;
    QueueSetMemberHandle_t activate_member = NULL;
    uint32_t task2_num = 0;
    while(1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY); // ѡ����м��е�һ������
        xQueueReceive(activate_member, &event_recv, portMAX_DELAY);
        event_val |= event_recv; // �ϲ��¼�ֵ
        lcd_show_xnum(182,110,event_val,1,16,0,BLUE);
        if(event_val == EVENTBIT_ALL) // �¼����а��������¼�
        {
            event_val = 0; // ����¼���
            lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);
        }
    }
}
/*---------------------------------------------*/
