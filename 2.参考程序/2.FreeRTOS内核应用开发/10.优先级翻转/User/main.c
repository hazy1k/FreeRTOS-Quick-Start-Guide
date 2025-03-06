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

static TaskHandle_t LowPriority_Task_Handle = NULL; // �����ȼ�������
static TaskHandle_t MidPriority_Task_Handle = NULL; // �����ȼ�������
static TaskHandle_t HighPriority_Task_Handle = NULL; // �����ȼ�������

/* �ں˶����� */
SemaphoreHandle_t BinarySem_Handle = NULL; // ��ֵ�ź������
/* ȫ�ֱ������� */
/* �궨�� */


/* ���������� */
static void AppTaskCreate(void); // ����������

static void LowPriority_Task(void* pvParameters); // �����ȼ�������
static void MidPriority_Task(void* pvParameters); // �����ȼ�������
static void HighPriority_Task(void* pvParameters); // �����ȼ�������

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
    printf("FreeRTOS ���ȼ���תʵ��\r\n");
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

  BinarySem_Handle = xSemaphoreCreateBinary(); // ������ֵ�ź���
  if(BinarySem_Handle != NULL)
  {
    printf("��ֵ�ź��������ɹ���\r\n");
  }
  xReturn = xSemaphoreGive(BinarySem_Handle); // �ͷŶ�ֵ�ź���

  // ���������ȼ�����
  xReturn = xTaskCreate((TaskFunction_t )LowPriority_Task,
                        (const char*)"LowPriority_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2, 
                        (TaskHandle_t*)&LowPriority_Task_Handle); 
  if(xReturn == pdPASS)
  {
      printf("�����ȼ����񴴽��ɹ���\r\n");
  }
  else
  {
    printf("�����ȼ����񴴽�ʧ�ܣ�\r\n");
  }

  // ���������ȼ�����
  xReturn = xTaskCreate((TaskFunction_t )MidPriority_Task,
                        (const char*)"MidPriority_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3, 
                        (TaskHandle_t*)&MidPriority_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("�����ȼ����񴴽��ɹ���\r\n");
  }
  else
  {
    printf("�����ȼ����񴴽�ʧ�ܣ�\r\n");
  }

  // ���������ȼ�����
  xReturn = xTaskCreate((TaskFunction_t )HighPriority_Task,
                        (const char*)"HighPriority_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)4, 
                        (TaskHandle_t*)&HighPriority_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("�����ȼ����񴴽��ɹ���\r\n");
  }
  else
  {
    printf("�����ȼ����񴴽�ʧ�ܣ�\r\n");
  }
  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // �˳��ٽ���
}

// �����ȼ�������
static void LowPriority_Task(void* pvParameters)
{
  static uint32_t i;
  BaseType_t xReturn = pdPASS;
  while(1)
  {
    printf("�����ȼ���ȡ�ź�����...\r\n");
    xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY); // �����ȼ������ȡ�ź���
    if(xReturn == pdTRUE)
    {
      printf("�����ȼ���ȡ�ź����ɹ���\r\n");
    }
    for(i = 0; i < 1000000; i++) // ģ������ȼ�����ʱ��ռ���ź���
    {
      taskYIELD(); // �������
    }
    printf("�����ȼ��ͷ��ź���\r\n");
    xReturn = xSemaphoreGive(BinarySem_Handle); // �����ȼ������ͷ��ź���
    LED1_TOGGLE();
    vTaskDelay(500);
  }
}

// �����ȼ�������
static void MidPriority_Task(void* pvParameters)
{
  while(1)
  {
    printf("�����ȼ�����������...\r\n");
    vTaskDelay(500);
  }
}

// �����ȼ�������
static void HighPriority_Task(void* pvParameters)
{
  BaseType_t xReturn = pdTRUE;
  while(1)
  {
    printf("�����ȼ������ȡ�ź���\r\n");
    xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY); // �����ȼ����������ȡ�ź���
    if(xReturn == pdTRUE)
    {
      printf("�����ȼ���ȡ�ź����ɹ���\r\n");
    }
    LED1_TOGGLE();
    xReturn = xSemaphoreGive(BinarySem_Handle); // �����ȼ������ͷ��ź���
    vTaskDelay(500);
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
