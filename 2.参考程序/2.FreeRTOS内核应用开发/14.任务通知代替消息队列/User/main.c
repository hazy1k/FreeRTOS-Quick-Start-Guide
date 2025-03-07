/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* ����ͷ�ļ� */
#include "uart.h"
#include "led.h"
#include "key.h"
#include "limits.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������

static TaskHandle_t Receive1_Task_Handle = NULL; // ��������1���
static TaskHandle_t Receive2_Task_Handle = NULL; // ��������2���
static TaskHandle_t Send_Task_Handle = NULL; // ����������

/* �ں˶����� */
/* ȫ�ֱ������� */
/* �궨�� */
#define USE_CHAR 0 // �����ַ�����ʱ������Ϊ1 ���Ա�����ʱ������Ϊ0

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
    printf("����һ��FreeRTOS����֪ͨ������Ϣ���еĲ��Գ���\r\n");
    printf("����KEY1����KEY2����������Ϣ֪ͨ\r\n");
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
  BaseType_t xReturn = pdTRUE;
#if USE_CHAR
  char *r_char;
#else
  uint32_t r_num;
#endif
  while(1)
  {
    //��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
    xReturn = xTaskNotifyWait(0x0,     // ���뺯����ʱ��������� bit
                              ULONG_MAX, // �˳�������ʱ��������е� bit 
    #if USE_CHAR
                              (uint32_t *)&r_char, // ��������ֵ֪ͨ
    #else
                              &r_num, // ��������ֵ֪ͨ
    #endif
                              portMAX_DELAY); // ����ʱ��
    if(xReturn == pdTRUE)
    {
    #if USE_CHAR
      printf("Receive1_Task ����֪ͨΪ %s \n",r_char);
    #else
      printf("Receive1_Task ����֪ͨΪ %d \n",r_num);
    #endif
    }
    LED1_TOGGLE(); 
  }
}

// ��������2����
static void Receive2_Task(void* pvParameters)
{
  BaseType_t xReturn = pdTRUE;
#if USE_CHAR
  char *r_char;
#else
  uint32_t r_num;
#endif
  while(1)
  {
    //��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
    xReturn = xTaskNotifyWait(0x0,     // ���뺯����ʱ��������� bit
                              ULONG_MAX, // �˳�������ʱ��������е� bit 
    #if USE_CHAR
                              (uint32_t *)&r_char, // ��������ֵ֪ͨ
    #else
                              &r_num, // ��������ֵ֪ͨ
    #endif
                              portMAX_DELAY); // ����ʱ��
    if(xReturn == pdTRUE)
    {
    #if USE_CHAR
      printf("Receive2_Task ����֪ͨΪ %s \n",r_char);
    #else
      printf("Receive2_Task ����֪ͨΪ %d \n",r_num);
    #endif
    }
    LED2_TOGGLE(); 
  }
}

// ����������
static void Send_Task(void* pvParameters)
{
  BaseType_t xReturn = pdPASS;
#if USE_CHAR
  char test_str1[] = "This is a test 1";
  char test_str2[] = "This is a test 2";
#else
  uint32_t test_num1 = 100;
  uint32_t test_num2 = 200;
#endif
  while(1)
  {
    /* KEY1�����£����������1����֪ͨ */
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotify(Receive1_Task_Handle, // ֪ͨ��������1
      #if USE_CHAR
                            (uint32_t)&test_str1, // ֵ֪ͨ
      #else
                            test_num1, // ֵ֪ͨ
      #endif 
                            eSetValueWithOverwrite); // ���ǵ�ǰ֪ͨ
      if(xReturn == pdPASS)
      {
        printf("Receive1_Task ����֪ͨ�ͷųɹ�!\r\n");
      }                     
    }
    /* KEY2�����£����������2����֪ͨ */
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotify(Receive2_Task_Handle, // ֪ͨ��������2
      #if USE_CHAR
                            (uint32_t)&test_str2, // ֵ֪ͨ
      #else
                            test_num2, // ֵ֪ͨ
      #endif 
                            eSetValueWithOverwrite); // ���ǵ�ǰ֪ͨ
      if(xReturn == pdPASS)
      {
        printf("Receive2_Task ����֪ͨ�ͷųɹ�!\r\n");
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
