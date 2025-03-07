/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* ����ͷ�ļ� */
#include "uart.h"
#include "led.h"
#include "key.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������

static TaskHandle_t Receive1_Task_Handle = NULL; // ��������1���
static TaskHandle_t Receive2_Task_Handle = NULL; // ��������2���
static TaskHandle_t Send_Task_Handle = NULL; // ����������

/* �ں˶����� */
/* ȫ�ֱ������� */
/* �궨�� */

/* ���������� */
static void AppTaskCreate(void); // ����������

static void Receive1_Task(void* pvParameters); // ��������1����
static void Receive2_Task(void* pvParameters); // ��������2����
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
    printf("����һ��FreeRTOS����֪ͨ�����ֵ�ź����Ĳ��Գ���\r\n");
    printf("����KEY1����KEY2����������������֮���ͬ��\r\n");
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

  xReturn = xTaskCreate((TaskFunction_t )Receive1_Task,
                        (const char*)"Receive1_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2,
                        (TaskHandle_t*)&Receive1_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("������������1�ɹ�\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )Receive2_Task,
                        (const char*)"Receive2_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&Receive2_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("������������2�ɹ�\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )Send_Task,
                        (const char*)"Send_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)4,
                        (TaskHandle_t*)&Send_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("������������ɹ�\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // �˳��ٽ���
}

// ��������1����
static void Receive1_Task(void* pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // �ȴ�֪ͨ
    printf("��������1�յ�֪ͨ\r\n");
    LED1_TOGGLE();
  }
}

// ��������2����
static void Receive2_Task(void* pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // �ȴ�֪ͨ
    printf("��������2�յ�֪ͨ\r\n");
    LED2_TOGGLE();
  }
}

static void Send_Task(void* pvParameters)
{
  BaseType_t xReturn = pdPASS;
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotifyGive(Receive1_Task_Handle); // ����֪ͨ����������1
      if(xReturn == pdPASS)
      {
        printf("�����������������1�ɹ�\r\n");
      }
    }
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotifyGive(Receive2_Task_Handle); // ����֪ͨ����������2
      if(xReturn == pdPASS)
      {
        printf("�����������������2�ɹ�\r\n");
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
