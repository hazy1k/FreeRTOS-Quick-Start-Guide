/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

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

TimerHandle_t TIM1_Handler; // �����ʱ�����
TimerHandle_t TIM2_Handler;
void Timer1_Callback(TimerHandle_t xTimer); // �����ʱ���ص�����
void Timer2_Callback(TimerHandle_t xTimer);
/*---------------------------------------------*/

/*------------------����ʵ����------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Timer",RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Timer1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Timer2: 000", BLUE);
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
    // TIM1Ϊ���ڶ�ʱ��
    TIM1_Handler = xTimerCreate((const char*)"TIM1",                      // ��ʱ������
                                (TickType_t)1000,                         // ��ʱ����ʱʱ��
                                (UBaseType_t)pdTRUE,                      // �����Զ�ʱ��
                                (void*)1,                                 // ��ʱ��ID
                                (TimerCallbackFunction_t)Timer1_Callback);// ��ʱ���ص�����
    // TIM2Ϊ���ζ�ʱ��
    TIM2_Handler = xTimerCreate((const char*)"TIM2",                     
                                (TickType_t)1000,                         
                                (UBaseType_t)pdFALSE,                     
                                (void*)2,                                 
                                (TimerCallbackFunction_t)Timer2_Callback);
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
    uint8_t key_value = 0;
    while(1)
    {
        if((TIM1_Handler != NULL) && (TIM2_Handler != NULL))
        {
            key_value = key_scan(0);
            switch(key_value)
            {
                case KEY0_PRES: // KEY0���������ʱ��
                {
                    xTimerStart((TimerHandle_t)TIM1_Handler,// �������Ķ�ʱ��
                                (TickType_t)portMAX_DELAY); // �ȴ�ϵͳ������ʱ��
                    xTimerStart((TimerHandle_t)TIM2_Handler,
                                (TickType_t)portMAX_DELAY);
                    break;
                }
                case KEY1_PRES: // KEY1ֹͣ�����ʱ��
                {
                    xTimerStop((TimerHandle_t)TIM1_Handler,
                                (TickType_t)portMAX_DELAY);
                    xTimerStop((TimerHandle_t)TIM2_Handler,
                                (TickType_t)portMAX_DELAY);
                    break;
                }
                default:break;
            }
        }
        vTaskDelay(10);
    }
}

// Timer1_Callback����ʵ��
void Timer1_Callback(TimerHandle_t xTimer)
{
    static uint32_t tim1_num = 0;
    lcd_fill(6, 131, 114, 313, lcd_discolor[++tim1_num % 11]);    /* LCD����ˢ�� */
    lcd_show_xnum(79, 111, tim1_num, 3, 16, 0x80, BLUE);          /* ��ʾ��ʱ��1��ʱ���� */
}

// Timer2_Callback����ʵ��
void Timer2_Callback(TimerHandle_t xTimer)
{
    static uint32_t tim2_num = 0;
    lcd_fill(126, 131, 233, 313, lcd_discolor[++tim2_num % 11]);  /* LCD����ˢ�� */
    lcd_show_xnum(199, 111, tim2_num, 3, 16, 0x80, BLUE);         /* ��ʾ��ʱ��2��ʱ���� */
}

/*---------------------------------------------*/
