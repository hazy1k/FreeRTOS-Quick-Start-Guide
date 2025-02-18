#include "FreeRTOS.h"
#include "task.h"
#include "bsp_led.h"
#include "bsp_usart.h"

// ����������
static TaskHandle_t AppTaskCreate_Handle = NULL;
// LED1������
static TaskHandle_t LED1_Task_Handle = NULL;
// LED2������
static TaskHandle_t LED2_Task_Handle = NULL;

// ��������
static void AppTaskCreate(void); // ���ڴ�������
static void LED1_Task(void* pvParameters);// LED1_Task����ʵ�� 
static void LED2_Task(void* pvParameters);// LED2_Task����ʵ�� 
static void BSP_Init(void);  // ���ڳ�ʼ�����������Դ

// ������
int main(void)
{	
  BaseType_t xReturn = pdPASS; // ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
  // ������Ӳ����ʼ��
  BSP_Init();
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS-��̬����������ʵ��!\r\n");
  // ����AppTaskCreate����
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,// ������ں��� 
                        (const char*)"AppTaskCreate", // �������� 
                        (uint16_t)512,  // ����ջ��С
                        (void*)NULL,    // ������ں������� 
                        (UBaseType_t)1, // ��������ȼ� 
                        (TaskHandle_t*)&AppTaskCreate_Handle);// ������ƿ�ָ��  
  // �����������           
  if(pdPASS == xReturn)
    vTaskStartScheduler();// �������񣬿������� 
  else
    return -1;  
  while(1);   /* ��������ִ�е����� */    
}

// ���񴴽�����
// Ϊ�˷���������е����񴴽����������������������
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS; // ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
  taskENTER_CRITICAL();        // �����ٽ���
  // ����LED1_Task����
  xReturn = xTaskCreate((TaskFunction_t)LED1_Task,// ������ں��� 
                        (const char*)"LED1_Task", // �������� 
                        (uint16_t)512,   // ����ջ��С 
                        (void*)NULL,	   // ������ں������� 
                        (UBaseType_t)2,	 // ��������ȼ� 
                        (TaskHandle_t*)&LED1_Task_Handle);// ������ƿ�ָ��
  if(pdPASS == xReturn)
    printf("����LED1_Task����ɹ�!\r\n");
	// ����LED2_Task����
  xReturn = xTaskCreate((TaskFunction_t)LED2_Task,// ������ں��� 
                        (const char*)"LED2_Task", // �������� 
                        (uint16_t)512,   // ����ջ��С
                        (void*)NULL,	   // ������ں������� 
                        (UBaseType_t)3,	 // ��������ȼ� 
                        (TaskHandle_t*)&LED2_Task_Handle);// ������ƿ�ָ�� 
  if(pdPASS == xReturn)
    printf("����LED2_Task����ɹ�!\r\n");
  vTaskDelete(AppTaskCreate_Handle); // ɾ��AppTaskCreate����
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

// LED1_Task����ʵ�� 
static void LED1_Task(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        vTaskDelay(500);
        printf("LED1_Task Running,LED1_ON\r\n");
        LED1_OFF;     
        vTaskDelay(500);    		
        printf("LED1_Task Running,LED1_OFF\r\n");
    }
}

// LED2_Task����ʵ�� 
static void LED2_Task(void* parameter)
{	
    while (1)
    {
        LED2_ON;
        vTaskDelay(500);   
        printf("LED2_Task Running,LED2_ON\r\n");
        
        LED2_OFF;     
        vTaskDelay(500);   	 		
        printf("LED2_Task Running,LED2_OFF\r\n");
    }
}

// ���������Դ��ʼ������
static void BSP_Init(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
	// LED ��ʼ�� 
	LED_GPIO_Config();
	// ���ڳ�ʼ��
	USART_Config();
}
