/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 
/* ����ͷ�ļ� */
#include "uart.h"
#include "led.h"
#include "key.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������

static TaskHandle_t Receive_Task_Handle = NULL; // ����������
static TaskHandle_t Send_Task_Handle = NULL; // ����������

/* �ں˶����� */
QueueHandle_t Test_Queue = NULL; // ��Ϣ���о��
/* ȫ�ֱ������� */

/* �궨�� */
#define QUEUE_LEN 4  // ��Ϣ���г��ȣ����ɰ���������Ϣ
#define QUEUE_SIZE 4 // ������ÿ����Ϣ�Ĵ�С

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
    printf("����KEY1����KEY2���Ͷ�����Ϣ\r\n");
    printf("Receive Task ���յ���Ϣ���ṩ���ڻ���\r\n");
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
    Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN, (UBaseType_t)QUEUE_SIZE); // ������Ϣ����
    if(Test_Queue != NULL)
    {
        printf("��Ϣ���д����ɹ���\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Receive_Task, // ������ں���
                         (const char*)"Receive_Task", // ��������
                         (uint16_t)512, // �����ջ��С
                         (void*)NULL, // �������
                         (UBaseType_t)2, // �������ȼ�
                         (TaskHandle_t*)&Receive_Task_Handle); // ������ƿ�
    if(xReturn == pdPASS)
    {
        printf("�������񴴽��ɹ���\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t )Send_Task, // ������ں���
                         (const char*)"Send_Task", // ��������
                         (uint16_t)1024, // �����ջ��С
                         (void*)NULL, // �������
                         (UBaseType_t)3, // �������ȼ�
                         (TaskHandle_t*)&Send_Task_Handle); // ������ƿ�
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
    BaseType_t xReturn = pdTRUE;
    uint32_t r_queue; // ������Ϣ��ʱ����
    while(1)
    {
        xReturn = xQueueReceive(Test_Queue, &r_queue, portMAX_DELAY); // ������Ϣ
        if(xReturn == pdTRUE)
        {
            printf("���յ���Ϣ��%d\r\n", r_queue); // ��ӡ���յ�����Ϣ
        }
        else
        {
            printf("���ճ���������룺%d\n", xReturn);
        }
    }
}

// ����������
static void Send_Task(void* pvParameters)
{
    BaseType_t xReturn = pdPASS;
    uint32_t send_data1 = 1; // Ҫ���͵���Ϣ
    uint32_t send_data2 = 2;
    while(1)
    {
        if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            printf("KEY1���£�����send_data1");
            xReturn = xQueueSend(Test_Queue, &send_data1, 0); // ������Ϣ
            if(xReturn == pdPASS)
            {
                printf("send_data1���ͳɹ���\r\n");
            }
        }
        if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            printf("KEY2���£�����send_data2");
            xReturn = xQueueSend(Test_Queue, &send_data2, 0); // ������Ϣ
            if(xReturn == pdPASS)
            {
                printf("send_data2���ͳɹ���\r\n");
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
