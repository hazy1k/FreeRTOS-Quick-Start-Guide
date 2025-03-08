/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* ����ͷ�ļ� */
#include "uart.h"
#include "led.h"
#include "key.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������

static TaskHandle_t LED_Task_Handle = NULL; // LED������
static TaskHandle_t RAM_Task_Handle = NULL; // �ڴ����������

/* �ں˶����� */
/* ȫ�ֱ������� */
uint8_t *Test_Ptr = NULL;
/* �궨�� */

/* ���������� */
static void AppTaskCreate(void); // ����������

static void LED_Task(void* pvParameters);
static void RAM_Task(void* pvParameters);

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
    printf("FreeRTOS�ڴ����ʵ��\r\n");
    printf("����KEY1�����ڴ棬����KEY2�ͷ��ڴ档\r\n");
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

  xReturn = xTaskCreate((TaskFunction_t )LED_Task,
                        (const char*)"LED_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2, 
                        (TaskHandle_t*)&LED_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("LED���񴴽��ɹ���\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )RAM_Task,
                        (const char*)"RAM_Task",  
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&RAM_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("�ڴ�������񴴽��ɹ���\r\n");
  }                       

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // �˳��ٽ���
}

static void LED_Task(void* pvParameters)
{
  while(1)
  {
    LED1_ON();
    vTaskDelay(1000);
    LED1_OFF();
    vTaskDelay(1000);
  }
}

static void RAM_Task(void* pvParameters)
{
  uint32_t g_memsize; // �ڴ��С
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      if(Test_Ptr == NULL) // �ڴ�δ����
      {
        g_memsize = xPortGetFreeHeapSize(); // ��ȡ���ж��ڴ��С
        printf("ϵͳ��ǰ�ڴ��СΪ: %d �ֽ�, �����ڴ�...\r\n", g_memsize);
        Test_Ptr = pvPortMalloc(1024); // ����1024�ֽ��ڴ�
        if(Test_Ptr != NULL)
        {
          printf("�ڴ�����ɹ���\r\n");
          printf("���뵽���ڴ��ַΪ%#x\n",(int)Test_Ptr);
          g_memsize = xPortGetFreeHeapSize(); // ��ȡ���ж��ڴ��С
          printf("ϵͳ��ǰ�ڴ��СΪ(�������): %d �ֽڡ�\r\n", g_memsize);
          // ��Test_Ptrд�����ݣ���ǰϵͳʱ��
          sprintf((char*)Test_Ptr, "��ǰϵͳʱ��TickCount: %d\n", xTaskGetTickCount());
          printf("д�����ݳɹ���\r\n");
          printf("��������Ϊ��%s\r\n", (char*)Test_Ptr);
        }
      }
      else
      {
        printf("����KEY2�ͷ��ڴ档\r\n");
      }
    }
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      if(Test_Ptr != NULL)
      {
        printf("�ͷ��ڴ�...\r\n");
        vPortFree(Test_Ptr); // �ͷ��ڴ�
        Test_Ptr = NULL;
        printf("�ڴ��ͷųɹ���\r\n");
        g_memsize = xPortGetFreeHeapSize(); // ��ȡ���ж��ڴ��С
        printf("ϵͳ��ǰ�ڴ��СΪ(�ͷź�): %d �ֽڡ�\r\n", g_memsize);
      }
      else
      {
        printf("����KEY1�����ڴ档\r\n");
      }
    }
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
