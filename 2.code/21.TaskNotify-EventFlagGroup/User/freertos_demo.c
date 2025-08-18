/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
#include "led.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

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
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

#define EventBit_0 (1<<0)
#define EventBit_1 (1<<1)
#define EventBit_ALL (EventBit_0 | EventBit_1)
/*---------------------------------------------*/

/*------------------����ʵ����------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Notify Demo",RED);
    lcd_show_string(30,110,220,16,16,"Event Group Value: 0",BLUE);
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

void task1(void *pvParameters)
{
    uint8_t key_value =0;
    while(1)
    {
        if(Task1Task_Handler != NULL)
        {
            key_value = key_scan(0);
            switch(key_value)
            {
                case KEY0_PRES:
                {
                    xTaskNotify((TaskHandle_t)Task2Task_Handler, (uint32_t)EventBit_0, (eNotifyAction)eSetBits);
                    break;
                }
                case KEY1_PRES:
                {
                    xTaskNotify((TaskHandle_t)Task2Task_Handler, (uint32_t)EventBit_1, (eNotifyAction)eSetBits);
                    break;
                }
                default:break;
            }
        }
        vTaskDelay(10);
    }
}

// task2����ʵ��
void task2(void *pvParameters)
{
    uint32_t notify_value = 0;
    uint32_t event_value = 0;
    uint32_t task2_value = 0;
    while(1)
    {
        xTaskNotifyWait((uint32_t)0x00000000,      // ���뺯��ʱ�����������ֵ֪ͨ
                        (uint32_t)0xFFFFFFFF,      // �����˳�ʱ�������������ֵ֪ͨ
                        (uint32_t*)&notify_value,  // �ȴ�����ֵ֪ͨ
                        (TickType_t)portMAX_DELAY);
        if(notify_value & EventBit_0)
        {
            event_value |= EventBit_0;
        }
        else if(notify_value & EventBit_1)
        {
            event_value |= EventBit_1;
        }
        lcd_show_xnum(182,110,event_value,1,16,0,BLUE);
        if(event_value == EventBit_ALL)
        {
            event_value = 0;
             lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_value % 11]);
        }
    }
}
/*---------------------------------------------*/