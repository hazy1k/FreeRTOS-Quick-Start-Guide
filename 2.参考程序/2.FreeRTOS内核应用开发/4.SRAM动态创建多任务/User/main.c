#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "led.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������
static TaskHandle_t LED1_Task_Handler = NULL; // LED1������
static TaskHandle_t LED2_Task_Handler = NULL; // LED2������

/* �ں˶����� */
/* ȫ�ֱ������� */

/* ���������� */
static void AppTaskCreate(void); // ����������
static void LED1_Task(void* pvParameters); // LED1������
static void LED2_Task(void* pvParameters); // LED2������
static void BSP_Init(void); // �弶��ʼ������

// ��������������
/*
    1.BSP��ʼ��
    2.����APP����
    3.����FreeRTOS,��������
*/
int main(void)
{
    BaseType_t xReturn = pdPASS; // ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
    BSP_Init(); // �弶��ʼ��
    printf("FreeRTOS SRAM Dynamic Create Multi Task Example\r\n");
    // ����AppTaskCreate����   
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*    )"AppTaskCreate",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )1, 
                        (TaskHandle_t*)&AppTaskCreate_Handle); 
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // ����FreeRTOS
        printf("FreeRTOS Start Success\r\n");
    }
    else
    {
        return -1;
    }
    while(1);
}

// Ϊ�˷�������������񴴽���������AppTaskCreate��ʵ��
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL(); // �����ٽ���
    xReturn = xTaskCreate((TaskFunction_t)LED1_Task, // ������ں���
                         (const char*)"LED1_Task",   // ��������
                         (uint16_t)512,             // �����ջ��С
                         (void*)NULL,              // ������ں�������
                         (UBaseType_t)2,            // �������ȼ�
                         (TaskHandle_t*)&LED1_Task_Handler);// ������ƿ�ָ��                           
    if(xReturn == pdPASS)
    {
        printf("Create LED1_Task Success\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)LED2_Task, // ������ں���
                         (const char*)"LED2_Task",   // ��������
                         (uint16_t)512,             // �����ջ��С
                         (void*)NULL,              // ������ں�������
                         (UBaseType_t)3,            // �������ȼ�
                         (TaskHandle_t*)&LED2_Task_Handler);// ������ƿ�ָ��                           
    if(xReturn == pdPASS)
    {
        printf("Create LED2_Task Success\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle);
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// LED1������
static void LED1_Task(void* pvParameters)
{
    while(1)
    {
        LED1_ON();
        printf("LED1 ON\r\n");
        vTaskDelay(500);
        LED1_OFF();
        printf("LED2 OFF\r\n");
        vTaskDelay(500);
    }
}

// LED2������
static void LED2_Task(void* pvParameters)
{
    while(1)
    {
        LED2_ON();
        printf("LED2 ON\r\n");
        vTaskDelay(1000);
        LED2_OFF();
        printf("LED2 OFF\r\n");
    }
}

// �弶��ʼ������
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �����ж����ȼ�����4
    LED_Init();
    USART_Config();
}
