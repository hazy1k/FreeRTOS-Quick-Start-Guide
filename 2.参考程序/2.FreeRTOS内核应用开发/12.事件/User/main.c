/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
/* ����ͷ�ļ� */
#include "uart.h"
#include "led.h"
#include "key.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������

static TaskHandle_t LED_Task_Handle = NULL; // LED������
static TaskHandle_t KEY_Task_Handle = NULL; // KEY������

/* �ں˶����� */
static EventGroupHandle_t Event_Handle = NULL; // �¼�����
/* ȫ�ֱ������� */
/* �궨�� */
#define KEY1_EVENT (0x01 << 0) // �����¼������λ0
#define KEY2_EVENT (0x01 << 1) // �����¼������λ1

/* ���������� */
static void AppTaskCreate(void); // ����������

static void LED_Task(void* pvParameters); // LED������
static void KEY_Task(void* pvParameters); // KEY������

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
    printf("FreeRTOS�¼���־λʵ��\r\n");
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

  Event_Handle = xEventGroupCreate(); // �����¼���
  if(Event_Handle != NULL)
  {
    printf("�¼��鴴���ɹ�\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )LED_Task,
                        (const char*)"LED_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2, 
                        (TaskHandle_t*)&LED_Task_Handle); // ����LED����
  if(xReturn == pdPASS)
  {
    printf("LED���񴴽��ɹ�\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,
                        (const char*)"KEY_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2, 
                        (TaskHandle_t*)&KEY_Task_Handle); // ����KEY����
  if(xReturn == pdPASS)
  {
    printf("KEY���񴴽��ɹ�\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // �˳��ٽ���
}

// LED������
static void LED_Task(void* pvParameters)
{
  EventBits_t r_event;
  while(1)
  {
    r_event = xEventGroupWaitBits(Event_Handle, KEY1_EVENT | KEY2_EVENT, pdTRUE, pdTRUE, portMAX_DELAY);
    if(r_event & (KEY1_EVENT | KEY2_EVENT) == (KEY1_EVENT | KEY2_EVENT))
    {
      printf("KEY1��KEY2ͬʱ����\r\n");
      LED1_TOGGLE();
    }
    else{
      printf("�¼�����\r\n");
    }
  }
}

// KEY������
static void KEY_Task(void* pvParameters)
{
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      printf("KEY1����\r\n");
      xEventGroupSetBits(Event_Handle, KEY1_EVENT);
    }
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      printf("KEY2����\r\n");
      xEventGroupSetBits(Event_Handle, KEY2_EVENT);
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
