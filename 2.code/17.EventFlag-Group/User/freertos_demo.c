/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

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
#define TASK1_PRIO      2                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);

// TASK2����
#define TASK2_PRIO      4                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3����
#define TASK3_PRIO      3                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);

EventGroupHandle_t Event_handle;
#define EVENT0_BIT (1 << 0)     // �¼�λ
#define EVENT1_BIT (1 << 1)
#define EVENTALL_BIT (EVENT0_BIT | EVENT1_BIT) // �ȴ������¼�λ�ĺ�
/*---------------------------------------------*/

/*------------------����ʵ����------------------*/
void freertos_demo(void)
{
    lcd_show_string(10, 47, 220, 24, 24, "Event Group", RED);
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

    // �����¼���־��
    Event_handle = xEventGroupCreate(); // ������ƻָ�

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

void task1(void *pvParameters)
{
    uint8_t key_value; // �������ָ�
    while(1)
    {
        key_value = key_scan(0); // �������Ͳ����ָ�
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t)Event_handle, (EventBits_t)EVENT0_BIT); // ������¼�λ��ָ�
                break;
            }
            case KEY1_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t)Event_handle, (EventBits_t)EVENT1_BIT); // ������¼�λ��ָ�
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
    uint32_t task2_value = 0; // �������ͳ�ʼ���ָ�
    while(1)
    {
        xEventGroupWaitBits((EventGroupHandle_t)Event_handle,// �ȴ����¼���־�����ָ�
                            (EventBits_t)EVENTALL_BIT,       // �ȴ����¼���־��ָ�
                            (BaseType_t)pdTRUE,              // �����˳�ʱ����ȴ����¼�
                            (BaseType_t)pdTRUE,              // �ȴ� �ȴ��¼��е������¼�
                            (TickType_t)portMAX_DELAY);      // �ȴ�ʱ��
        lcd_fill(6,131,233,313,lcd_discolor[++task2_value%11]); // LCD����ˢ��
        vTaskDelay(10);
    }
}

// task3����ʵ��
void task3(void *pvParameters)
{
    EventBits_t event_value; // �������ָ�
    while(1)
    {
        event_value = xEventGroupGetBits((EventGroupHandle_t)Event_handle); 
        lcd_show_xnum(182,110,event_value,1,16,0,BLUE);
        vTaskDelay(10);
    }
}
/*---------------------------------------------*/