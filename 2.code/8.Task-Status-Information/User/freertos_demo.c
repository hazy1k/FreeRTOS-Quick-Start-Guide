/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include <string.h>
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
    vTaskDelete(StartTask_Handler); // ��ʼ�����Ѿ�����Լ�ʹ����ɾ���Լ�
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// task1����ʵ��
void task1(void *pvParameters)
{
    uint32_t i = 0;
    UBaseType_t task_num = 0;
    TaskStatus_t *status_array = NULL;
    TaskHandle_t task_handle = NULL;
    TaskStatus_t *task_info = NULL;
    eTaskState task_state = eInvalid;
    char *task_state_str = NULL;
    char *task_info_buf = NULL;

    /*--------------------����uxTaskGetSystemState()��ʹ��------==--------------*/
    printf("-------------Step1 ����uxTaskGetSystemState()��ʹ��-------------\r\n");
    task_num = uxTaskGetNumberOfTasks(); // ��ȡϵͳ����������
    status_array = mymalloc(SRAMIN, task_num*sizeof(TaskStatus_t)); // �ڴ����
    task_num = uxTaskGetSystemState((TaskStatus_t*)status_array, // ����״̬��Ϣ
                                    (UBaseType_t)task_num, // buffer��С
                                    (uint32_t*)NULL); // ����ȡ��������ʱ����Ϣ
    printf("������\t\t���ȼ�\t\t������\r\n");
    for(i=0;i<task_num;i++)
    {
        printf("%s\t%s%ld\t\t%ld\r\n",
        status_array[i].pcTaskName,
        strlen(status_array[i].pcTaskName) > 7 ? "": "\t",
        status_array[i].uxCurrentPriority,
        status_array[i].xTaskNumber);
    }   
    myfree(SRAMIN, status_array);     
    printf("-----------------------------END-------------------------------\r\n");
    printf("����KEYO�Լ���������\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    
    /*-------------------------����vTaskGetInfo()��ʹ��--------------------------*/
    printf("-----------------Step2 ����vTaskGetInfo()��ʹ��------------------\r\n");
    task_info = mymalloc(SRAMIN, sizeof(TaskStatus_t)); // �ڴ����
    task_handle = xTaskGetHandle("task1"); // ��ȡ������
    vTaskGetInfo((TaskHandle_t)task_handle, // ������
                 (TaskStatus_t*)task_info,  // ����״̬��Ϣ
                 (BaseType_t)pdTRUE,        // ����ͳ�������ջ��ʷ��Сֵ
                 (eTaskState)eInvalid);     // ����״̬
    printf("������:\t\t\t%s\r\n", task_info->pcTaskName);
    printf("������:\t\t%ld\r\n", task_info->xTaskNumber);
    printf("����׳̬:\t\t%d\r\n", task_info->eCurrentState);
    printf("����ǰ���ȼ�:\t\t%ld\r\n", task_info->uxCurrentPriority);
    printf("��������ȼ�:\t\t%ld\r\n", task_info->uxBasePriority);
    printf("�����ջ����ַ:\t\t0x%p\r\n", task_info->pxStackBase);
    printf("�����ջ��ʷʣ����Сֵ:\t%d\r\n", task_info->usStackHighWaterMark);
    myfree(SRAMIN, task_info);
    printf("-----------------------------END-------------------------------\r\n");
    printf("����KEYO�Լ���������\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /*-------------------------����eTaskGetState()��ʹ��--------------------------*/
    printf("-----------------Step3 ����eTaskGetState()��ʹ��------------------\r\n");
    task_state_str = mymalloc(SRAMIN, 10); // �ڴ����
    task_handle = xTaskGetHandle("task1");
    task_state = eTaskGetState(task_handle);
    sprintf(task_state_str, task_state == eRunning ? "Runing" :
                            task_state == eReady ? "Ready" :
                            task_state == eBlocked ? "Blocked" :
                            task_state == eSuspended ? "Suspended" :
                            task_state == eDeleted ? "Deleted" :
                            task_state == eInvalid ? "Invalid" :
                                                     "");
    printf("����״ֵ̬: %d����Ӧ״̬Ϊ: %s\r\n", task_state, task_state_str);
    myfree(SRAMIN, task_state_str);
    printf("-----------------------------END-------------------------------\r\n");
    printf("����KEYO�Լ���������\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /*---------------------------����vTaskList()��ʹ��--------------------------*/
    printf("------------------Step4 ����vTaskList()��ʹ��-------------------\r\n");
    task_info_buf = mymalloc(SRAMIN, 500);
    vTaskList(task_info_buf); // ��ӡ�����б�
    printf("������\t\t״̬\t���ȼ�\tʣ��ջ\t�������\r\n");
    printf("%s\r\n", task_info_buf);
    myfree(SRAMIN, task_info_buf);
    printf("-----------------------------END-------------------------------\r\n");
    while(1)
    {
        vTaskDelay(10);
    }
}
/*-------------------------------------------*/