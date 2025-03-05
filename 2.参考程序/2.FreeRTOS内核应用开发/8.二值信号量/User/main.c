/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 
#include "semphr.h"
/* ����ͷ�ļ� */
#include "uart.h"
#include "led.h"
#include "key.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������

static TaskHandle_t Receive_Task_Handle = NULL; // ����������
static TaskHandle_t Send_Task_Handle = NULL; // ����������

/* �ں˶����� */
SemaphoreHandle_t BinarySem_Handle = NULL; // ��ֵ�ź������
/* ȫ�ֱ������� */
/* �궨�� */


/* ���������� */
static void AppTaskCreate(void); // ����������

static void Receive_Task(void* pvParameters); // ����������
static void Send_Task(void* pvParameters); // ����������

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
    printf("����KEY1����KEY2����������������ͬ��\r\n");
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
        printf("AppTaskCreate ����ʧ�ܣ�\r\n");
        return -1;
    }
    while(1);
}

// Ϊ�˷�������������񴴽���������AppTaskCreate��ʵ��
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL(); // �����ٽ���

    BinarySem_Handle = xSemaphoreCreateBinary(); // ������ֵ�ź���
    if(BinarySem_Handle != NULL)
    {
        printf("��ֵ�ź��������ɹ���\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Receive_Task,
                         (const char*)"Receive_Task",
                         (uint16_t)512,  
                         (void*)NULL,
                         (UBaseType_t)2,
                         (TaskHandle_t*)&Receive_Task_Handle);
    if(xReturn == pdPASS)
    {
        printf("�������񴴽��ɹ���\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Send_Task,
                         (const char*)"Send_Task",
                         (uint16_t)512,  
                         (void*)NULL,
                         (UBaseType_t)3,
                         (TaskHandle_t*)&Send_Task_Handle);
    if(xReturn == pdPASS)
    {
        printf("�������񴴽��ɹ���\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle);

    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// ����������
static void Receive_Task(void* pvParameters)
{
    BaseType_t xReturn = pdPASS;
    while(1)
    {
        // ��ȡ��ֵ�ź���xSemaphore
        xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY);
        if(xReturn == pdTRUE)
        {
            printf("���������ȡ��ֵ�ź����ɹ���\r\n");
        }
        LED1_TOGGLE();
    }
}

// ����������
static void Send_Task(void* pvParameters)
{
    BaseType_t xReturn = pdPASS;
    while(1)
    {
        if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            xReturn = xSemaphoreGive(BinarySem_Handle); // ������ֵ�ź�
            if(xReturn == pdTRUE)
            {
                printf("��ֵ�ź��ͷųɹ���\r\n");
            }
            else
            {
                printf("��ֵ�ź��ͷ�ʧ�ܣ�\r\n");
            }
        }
        if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            xReturn = xSemaphoreGive(BinarySem_Handle);
            if(xReturn == pdTRUE)
            {
                printf("��ֵ�ź��ͷųɹ���\r\n");
            }
            else
            {
                printf("��ֵ�ź��ͷ�ʧ�ܣ�\r\n");
            }
        }
        vTaskDelay(20);
    }
}

// �弶��ʼ������
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �����ж����ȼ�����4
    LED_Init();
    USART_Config();
    Key_GPIO_Config();
}
