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
SemaphoreHandle_t CountSem_Handle = NULL;
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
    printf("��һ��FreeRTOS����֪ͨ��������ź���������\r\n");
    printf("��λĬ��10��������KEY1���복λ������KEY2�ͷų�λ\r\n");
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

  xReturn = xTaskCreate((TaskFunction_t )Take_Task,
                        (const char*)"Take_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2,
                        (TaskHandle_t*)&Take_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("����Take_Task�ɹ�\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )Give_Task,
                        (const char*)"Give_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&Give_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("����Give_Task�ɹ�\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // �˳��ٽ���
}

// Taske_Task���������복λ
static void Take_Task(void* pvParameters)
{
  uint32_t take_num = pdTRUE;
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      // ��ȡ����֪ͨ��û��ȡ���򲻵ȴ�
      take_num = ulTaskNotifyTake(pdFALSE, 0);
      if(take_num > 0)
      {
        printf("KEY1�����£��ɹ����뵽��λ����ǰ��λΪ %d\r\n", take_num-1);
      }
      else
      {
        printf("KEY1�����£���û�г�λ���á�\r\n");
      }
    }
    vTaskDelay(20);
  }
}

static void Give_Task(void* pvParameters)
{
  BaseType_t xReturn = pdPASS;
  while(1)
  {
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      xTaskNotifyGive(Take_Task_Handle); // ֪ͨTake_Task�����г�λ����
      if(xReturn == pdPASS)
      {
        printf("KEY2�����£��ɹ��ͷų�λ��\r\n");
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
