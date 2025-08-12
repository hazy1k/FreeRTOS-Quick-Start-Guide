/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

/*-------------ȫ�ֱ�����������������------------*/
// LCD��ʾ��ɫ
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};
/*---------------------------------------------*/

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

// TASK2����
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3����
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);
/*---------------------------------------------*/

/*----------------������ʵ����----------------*/
void freertos_demo(void)
{
    // ��ʼ��LCD��ʾ
    lcd_show_string(10, 47, 220, 30, 24, "Task Create&Delete", RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);

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
    // ����TASK3����
    xTaskCreate((TaskFunction_t)task3,      
                (const char*)"task3",       
                (uint16_t)TASK3_STK_SIZE,   
                (void*)NULL,               
                (UBaseType_t)TASK3_PRIO,    
                (TaskHandle_t*)&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); // ɾ��START_TASK����
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// task1����ʵ��
// ʵ�ֹ��ܣ���¼�Լ������д�����ÿ������ʱ��LCD��ʾ��ͬ��ɫ
void task1(void *pvParameters)
{
    uint32_t task1_count = 0;
    while(1)
    {
        lcd_fill(6,131,114,313,lcd_discolor[++task1_count % 11]);
        lcd_show_xnum(71,111,task1_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task2����ʵ��
// ʵ�ֹ��ܣ���¼�Լ������д�����ÿ������ʱ��LCD��ʾ��ͬ��ɫ
void task2(void *pvParameters)
{
    uint32_t task2_count = 0;
    while(1)
    {
        lcd_fill(126,131,233,313,lcd_discolor[11-(++task2_count % 11)]);
        lcd_show_xnum(191,111,task2_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task3����ʵ��
// ʵ�ֹ��ܣ�����KEY0ɾ��task1���񣬰���KEY1ɾ��task2����
void task3(void *pvParameters)
{
    uint8_t key_value;
    while(1)
    {
        key_value = key_scan(0);
        if(key_value == KEY0_PRES)
        {
            if(Task1Task_Handler != NULL)
            {
                vTaskDelete(Task1Task_Handler);
                Task1Task_Handler = NULL;
                lcd_show_string(15, 111, 110, 16, 16, "Task1: Del", RED); 
            }
        }
        if(key_value == KEY1_PRES)
        {
            if(Task2Task_Handler != NULL)
            {
                vTaskDelete(Task2Task_Handler);
                Task2Task_Handler = NULL;
                lcd_show_string(135, 111, 110, 16, 16, "Task2: Del", RED);
            }
        }
        vTaskDelay(10);
    }
}
/*---------------------------------------------*/
