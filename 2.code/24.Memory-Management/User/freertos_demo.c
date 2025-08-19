/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

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
/*---------------------------------------------*/

/*------------------����ʵ����------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Memory Management",RED);    
    lcd_show_string(30, 118, 200, 16, 16, "Total Mem:      Bytes", RED);
    lcd_show_string(30, 139, 200, 16, 16, "Free  Mem:      Bytes", RED);
    lcd_show_string(30, 160, 200, 16, 16, "Malloc Addr:", RED);
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
    vTaskDelete(StartTask_Handler); // ��ʼ�����Ѿ�����Լ�ʹ����ɾ���Լ�
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

void task1(void *pvParameters)
{
    uint8_t key_value = 0;
    uint8_t *buf = NULL;
    size_t free_size = 0;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                buf = pvPortMalloc(30); // ����30�ֽ��ڴ�
                sprintf((char*)buf, "0x%p",buf);
                lcd_show_string(130,160,200,16,16,(char*)buf,BLUE);
                break;
            }
            case KEY1_PRES:
            {
                if(NULL != buf)
                {
                    vPortFree(buf); // �ͷ��ڴ�
                    buf = NULL;
                }
                break;
            }
            default:break;
        }
        lcd_show_xnum(114,118,configTOTAL_HEAP_SIZE,5,16,0,BLUE); // ��ʾ���ڴ��С
        free_size = xPortGetFreeHeapSize(); // ��ȡ�����ڴ��С
        lcd_show_xnum(114,139,free_size,5,16,0,BLUE);
        vTaskDelay(10);
    }
}
/*---------------------------------------------*/