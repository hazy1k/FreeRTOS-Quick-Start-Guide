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

static TimerHandle_t Swtmr1_Handle = NULL; // �����ʱ��1���
static TimerHandle_t Swtmr2_Handle = NULL; // �����ʱ��2���

/* �ں˶����� */
/* ȫ�ֱ������� */
static uint32_t TmrCb_Count1 = 0; // ��¼�����ʱ��1�ص�����ִ�д���
static uint32_t TmrCb_Count2 = 0; // ��¼�����ʱ��2�ص�����ִ�д���
/* �궨�� */


/* ���������� */
static void AppTaskCreate(void); // ����������

static void Swtmr1_Callback(void* pvParameters); 
static void Swtmr2_Callback(void* pvParameters); 

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
    printf("FreeRTOS�����ʱ��ʵ��\r\n");
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
  taskENTER_CRITICAL(); // �����ٽ���

  Swtmr1_Handle = xTimerCreate((const char*)"AutoReloadTimer", // ��ʱ����������
                            (TickType_t)1000, // ��ʱ������1000(tick)
                            (UBaseType_t)pdTRUE, // ����ģʽ
                            (void*)1, // ÿ����ʱ������һ��������ΨһID
                            (TimerCallbackFunction_t)Swtmr1_Callback); // ��ʱ���ص�����
  if(Swtmr1_Handle != NULL)
  {
    xTimerStart(Swtmr1_Handle, 0); // ������ʱ��(����ģʽ)
  }
  Swtmr2_Handle = xTimerCreate((const char*)"OneShotTimer", // ��ʱ����������
                            (TickType_t)500, // ��ʱ������5000(tick)
                            (UBaseType_t)pdFALSE, // ����ģʽ
                            (void*)2, // ÿ����ʱ������һ��������ΨһID
                            (TimerCallbackFunction_t)Swtmr2_Callback); // ��ʱ���ص�����
  if(Swtmr2_Handle != NULL)
  {
    xTimerStart(Swtmr2_Handle, 0); // ������ʱ��(����ģʽ)
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // �˳��ٽ���
}


// ���ڶ�ʱ��1�ص�����
static void Swtmr1_Callback(void* pvParameters)
{
  TickType_t tick_num1;
  TmrCb_Count1++; // ��¼�����ʱ��1�ص�����ִ�д���
  tick_num1 = xTaskGetTickCount(); // ��ȡ�δ�ʱ���ļ���ֵ
  LED1_TOGGLE();
  printf("�����ʱ��1�ص�����ִ�д�����%d���δ�ʱ������ֵ��%d\r\n", TmrCb_Count1, tick_num1);
}

// ���ζ�ʱ��2�ص�����
static void Swtmr2_Callback(void* pvParameters)
{
  TickType_t tick_num2;
  TmrCb_Count2++;
  tick_num2 = xTaskGetTickCount(); // ��ȡ�δ�ʱ���ļ���ֵ
  printf("�����ʱ��2�ص�����ִ�д�����%d���δ�ʱ������ֵ��%d\r\n", TmrCb_Count2, tick_num2);
}

// �弶��ʼ������
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �����ж����ȼ�����4
    LED_Init();
    USART_Config();
    Key_GPIO_Config();
}
