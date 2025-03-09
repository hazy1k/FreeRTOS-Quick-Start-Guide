/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* ����ͷ�ļ� */
#include "uart.h"
#include "led.h"
#include "TiMbase.h"
#include "string.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handle = NULL; // ����������

static TaskHandle_t LED1_Task_Handle = NULL; // LED1������
static TaskHandle_t LED2_Task_Handle = NULL; // LED2������
static TaskHandle_t CPU_Task_Handle = NULL; // CPUʹ����������

/* �ں˶����� */
/* ȫ�ֱ������� */
/* �궨�� */

/* ���������� */
static void AppTaskCreate(void); // ����������

static void LED1_Task(void* pvParameters);
static void LED2_Task(void* pvParameters);
static void CPU_Task(void* pvParameters);

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
    printf("FreeRTOS CPUʹ����ͳ��Demo\r\n");
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

  xReturn = xTaskCreate((TaskFunction_t)LED1_Task,
                        (const char*)"LED1_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)2,
                        (TaskHandle_t*)&LED1_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("LED1���񴴽��ɹ���\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t)LED2_Task,
                        (const char*)"LED2_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&LED2_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("LED2���񴴽��ɹ���\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t)CPU_Task,
                        (const char*)"CPU_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)4,
                        (TaskHandle_t*)&CPU_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("CPUʹ�������񴴽��ɹ���\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // �˳��ٽ���
}

static void LED1_Task(void* pvParameters)
{
  while(1)
  {
    LED1_ON();
    vTaskDelay(500);
    printf("LED1_Task Running..., LED1_ON\r\n");
    LED1_OFF();
    vTaskDelay(500);
    printf("LED1_Task Running..., LED1_OFF\r\n");
  }
}

static void LED2_Task(void* pvParameters)
{
  while(1)
  {
    LED2_ON();
    vTaskDelay(1000);
    printf("LED2_Task Running..., LED2_ON\r\n");
    LED2_OFF();
    vTaskDelay(1000);
    printf("LED2_Task Running..., LED2_OFF\r\n");
  }
}
static void CPU_Task(void* parameter)
{
  uint8_t CPU_RunInfo[400]; // ������������ʱ����Ϣ
  while (1) 
  {
    memset(CPU_RunInfo,0,400); // ��Ϣ����������
    vTaskList((char*)&CPU_RunInfo); // ��ȡ��������ʱ����Ϣ
    printf("---------------------------------------------\r\n");
    printf("������ ����״̬ ���ȼ� ʣ��ջ �������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    memset(CPU_RunInfo,0,400); //��Ϣ����������
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    printf("������ ���м��� ʹ����\r\n"); 
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(500);
  }
}

// �弶��ʼ������
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �����ж����ȼ�����4
    LED_Init();
    USART_Config();
    BASIC_TIM_Init();
}
