/* �û�ͷ�ļ� */
#include "freertos_demo.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include <stdio.h>
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

/*----------------�б�������------------------*/
List_t TestList;      // �����б�
ListItem_t ListItem1; // �����б���1
ListItem_t ListItem2; // �����б���2
ListItem_t ListItem3; // �����б���3
/*-------------------------------------------*/

/*------------------����ʵ����----------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"List Insert Demo",RED);
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
    // ��ʼ���б���б���
    vListInitialise(&TestList);
    vListInitialiseItem(&ListItem1);
    vListInitialiseItem(&ListItem2);
    vListInitialiseItem(&ListItem3);

    // ��ӡ�б���б���ĵ�ַ
    printf("-----------------��ӡ�б���б���ĵ�ַ-------------------\r\n");
    printf("��Ŀ\t\t\t��ַ\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex); // �б�ͷ
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd)); // �б�β
    printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
    printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
    printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
    printf("-----------------------STEP1 END------------------------\r\n");
    printf("����KEY0�Լ���\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // �б���1�����б���
    printf("--------------------�б���1�����б���---------------------\r\n");
    vListInsert((List_t*)&TestList, (ListItem_t*)&ListItem1);
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("-----------------------STEP2 END------------------------\r\n");
    printf("����KEY0�Լ���\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // �б���2�����б���
    printf("--------------------�б���2�����б���---------------------\r\n");
    vListInsert((List_t*)&TestList, (ListItem_t*)&ListItem2);
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("-----------------------STEP3 END------------------------\r\n");
    printf("����KEY0�Լ���\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // �б���3�����б���
    printf("--------------------�б���3�����б���---------------------\r\n");
    vListInsert((List_t*)&TestList, (ListItem_t*)&ListItem3);
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("-----------------------STEP4 END------------------------\r\n");
    printf("����KEY0�Լ���\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // �Ƴ��б���2
    printf("------------------------�Ƴ��б���2-----------------------\r\n");
    uxListRemove((ListItem_t*)&ListItem2);
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("����KEY0�Լ���\r\n");
    printf("-----------------------STEP5 END------------------------\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

   // ���б�ĩβ����б���2
    printf("-------------------���б�ĩβ����б���2-------------------\r\n");
    vListInsertEnd((List_t*)&TestList, (ListItem_t*)&ListItem2);
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("-----------------------------END-------------------------\r\n");
    while(1)
    {
        vTaskDelay(10);
    }
}
/*-------------------------------------------*/