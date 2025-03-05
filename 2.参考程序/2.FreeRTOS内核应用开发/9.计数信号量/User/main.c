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

static TaskHandle_t Take_Task_Handle = NULL; 
static TaskHandle_t Give_Task_Handle = NULL; 

/* �ں˶����� */
SemaphoreHandle_t CountSem_Handle = NULL; // �����ź������
/* ȫ�ֱ������� */
/* �궨�� */


/* ���������� */
static void AppTaskCreate(void); // ����������

static void Take_Task(void* pvParameters);
static void Give_Task(void* pvParameters); 

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
    printf("��λĬ��ֵ5��������KEY1���복λ��Ҳ���Ǹ��ź�����������KEY2�ͷų�λ��Ҳ�����ͷ��ź�����\r\n");
    // ����AppTaskCreate����   
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*)"AppTaskCreate",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)1, 
                        (TaskHandle_t*)&AppTaskCreate_Handle); 
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // ����FreeRTOS
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

    CountSem_Handle = xSemaphoreCreateCounting(5,5); // ���������ź�������ʼֵΪ5�����ֵΪ5
    if(CountSem_Handle != NULL)
    {
        printf("�����ź��������ɹ���\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Take_Task,
                         (const char*)"Take_Task",
                         (uint16_t)512,  
                         (void*)NULL,
                         (UBaseType_t)2,
                         (TaskHandle_t*)&Take_Task_Handle);
    if(xReturn == pdPASS)
    {
        printf("�������񴴽��ɹ���\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Give_Task,
                         (const char*)"Give_Task",
                         (uint16_t)512,  
                         (void*)NULL,
                         (UBaseType_t)3,
                         (TaskHandle_t*)&Give_Task_Handle);
    if(xReturn == pdPASS)
    {
        printf("�ͷ����񴴽��ɹ���\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle);

    taskEXIT_CRITICAL(); // �˳��ٽ���
}

// ����������
static void Take_Task(void* pvParameters)
{
    BaseType_t xReturn = pdTRUE;
    while(1)
    {
        if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            xReturn = xSemaphoreTake(CountSem_Handle, 0); // ��������ź���
            if(xReturn == pdTRUE)
            {
                printf("KEY1�����£�����һ����λ�ɹ���\r\n");
            }
            else
            {
                printf("��ȻKEY1�����£����ǳ�λ�Ѿ����ˣ�\r\n");
            }
        }
        vTaskDelay(20);
    }
}

// �ͷ�������
static void Give_Task(void* pvParameters)
{
    BaseType_t xReturn = pdTRUE;
    while(1)
    {
        if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            xReturn = xSemaphoreGive(CountSem_Handle); // �ͷż����ź���
            if(xReturn == pdTRUE)
            {
                printf("KEY2�����£��ͷ�һ����λ�ɹ���\r\n");
            }
            else
            {
                printf("��ȻKEY2�����£����ǳ�λ�Ѿ����ˣ�\r\n");
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
