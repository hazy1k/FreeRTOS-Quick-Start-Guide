#include "FreeRTOS.h"
#include "task.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;// ���������� 
static TaskHandle_t LED_Task_Handle = NULL;// LED������ 
static TaskHandle_t KEY_Task_Handle = NULL;// KEY������ 
static void AppTaskCreate(void); // ���ڴ�������
static void LED_Task(void* pvParameters);// LED_Task����ʵ�� 
static void KEY_Task(void* pvParameters);// KEY_Task����ʵ�� 
static void BSP_Init(void); // ���ڳ�ʼ�����������Դ 

int main(void)
{	
  BaseType_t xReturn = pdPASS; // ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
  // ������Ӳ����ʼ��
  BSP_Init();
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�������ʵ�飡\n\n");
  printf("����KEY1�������񣬰���KEY2�ָ�����\n");
  // ����AppTaskCreate����
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate, // ������ں��� 
                        (const char*)"AppTaskCreate",  // �������� 
                        (uint16_t)512,  // ����ջ��С 
                        (void*)NULL,    // ������ں������� 
                        (UBaseType_t)1, // ��������ȼ� 
                        (TaskHandle_t*)&AppTaskCreate_Handle);// ������ƿ�ָ��  
  // �����������          
  if(pdPASS == xReturn)
    vTaskStartScheduler(); // �������񣬿�������
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}

// Ϊ�˷���������е����񴴽����������������������
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS; // ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
  taskENTER_CRITICAL();        // �����ٽ���
  // ����LED_Task����
  xReturn = xTaskCreate((TaskFunction_t)LED_Task, // ������ں��� 
                        (const char*)"LED_Task",  // �������� 
                        (uint16_t)512,            // ����ջ��С
                        (void*)NULL,	            // ������ں������� 
                        (UBaseType_t)2,	          // ��������ȼ� 
                        (TaskHandle_t*)&LED_Task_Handle);// ������ƿ�ָ�� 
  if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
  // ����KEY_Task����
  xReturn = xTaskCreate((TaskFunction_t)KEY_Task, // ������ں���
                        (const char*)"KEY_Task",  // �������� 
                        (uint16_t)512,            // ����ջ��С 
                        (void*)NULL,              // ������ں������� 
                        (UBaseType_t)3,           // ��������ȼ� 
                        (TaskHandle_t*)&KEY_Task_Handle);// ������ƿ�ָ��  
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
  vTaskDelete(AppTaskCreate_Handle); // ɾ��AppTaskCreate����
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

// LED_Task����ʵ��
static void LED_Task(void* parameter)
{	
  while (1)
  {
    LED1_ON;
    printf("LED_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   // ��ʱ500��tick
    LED1_OFF;     
    printf("LED_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);  // ��ʱ500��tick 
  }
}

// KEY_Task����ʵ��
static void KEY_Task(void* parameter)
{	
  while (1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
    { 
      // K1 ������
      printf("����LED����\n");
      vTaskSuspend(LED_Task_Handle); // ����LED����
      printf("����LED����ɹ���\n");
    } 
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
    { 
      // K2 ������
      printf("�ָ�LED����\n");
      vTaskResume(LED_Task_Handle); // �ָ�LED����
      printf("�ָ�LED����ɹ���\n");
    }
    vTaskDelay(20); // ��ʱ20��tick
  }
}

// ������Ӳ����ʼ��
static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	// LED ��ʼ�� 
	LED_GPIO_Config();
	// ���ڳ�ʼ��	
	USART_Config();
  // ������ʼ��
  Key_GPIO_Config();
}
