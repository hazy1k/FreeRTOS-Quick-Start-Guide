#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "led.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������
static TaskHandle_t LED_Task_Handler = NULL; // LED������

/* �ں˶����� */
/* ȫ�ֱ������� */

/* ���������� */
static void AppTaskCreate(void); // ����������
static void LED_Task(void* pvParameters); // LED������
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
    printf("FreeRTOS SRAM Dynamic Create Single Task Example\r\n");
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
    xReturn = xTaskCreate((TaskFunction_t)LED_Task, // ������ں���
                         (const char*)"LED_Task",   // ��������
                         (uint16_t)512,             // �����ջ��С
                         (void*)NULL,              // ������ں�������
                         (UBaseType_t)2,            // �������ȼ�
                         (TaskHandle_t*)&LED_Task_Handler);// ������ƿ�ָ��                           
    if(xReturn == pdPASS)
    {
        printf("Create LED_Task Success\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle); // ɾ��AppTaskCreate����
    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// LED������
static void LED_Task(void* pvParameters)
{
    while(1)
    {
        LED1_ON();
        printf("LED ON\r\n");
        vTaskDelay(500);
        LED1_OFF();
        printf("LED OFF\r\n");
        vTaskDelay(500);
    }
}

// �弶��ʼ������
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �����ж����ȼ�����4
    LED_Init();
    USART_Config();
}
