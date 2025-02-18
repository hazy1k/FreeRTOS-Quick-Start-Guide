#include "FreeRTOS.h"
#include "task.h"
#include "bsp_led.h"
#include "bsp_usart.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */

// ����������
static TaskHandle_t AppTaskCreate_Handle;
// LED������ 
static TaskHandle_t LED_Task_Handle;		

// AppTaskCreate���������ջ
static StackType_t AppTaskCreate_Stack[128];
// LED�����ջ 
static StackType_t LED_Task_Stack[128];
// AppTaskCreate ������ƿ�
static StaticTask_t AppTaskCreate_TCB;
// AppTaskCreate ������ƿ� 
static StaticTask_t LED_Task_TCB;
// �������������ջ 
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
// ��ʱ�������ջ 
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
// ����������ƿ� 
static StaticTask_t Idle_Task_TCB;	
// ��ʱ��������ƿ� 
static StaticTask_t Timer_Task_TCB;
 

static void AppTaskCreate(void); // ���ڴ������� 
static void LED_Task(void* pvParameters); // LED_Task����ʵ�� 
static void BSP_Init(void); // ���ڳ�ʼ�����������Դ 

/**
	* ʹ���˾�̬�����ڴ棬�������������������û�ʵ�֣�������task.c�ļ���������
	*	���ҽ��� configSUPPORT_STATIC_ALLOCATION ����궨��Ϊ 1 ��ʱ�����Ч
	*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
																		StackType_t **ppxTimerTaskStackBuffer, 
																		uint32_t *pulTimerTaskStackSize); // ��ȡ��ʱ������������ջ��������ƿ��ڴ�

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize); // ��ȡ��������������ջ��������ƿ��ڴ�
/*
  ��һ����������Ӳ����ʼ�� 
  �ڶ���������APPӦ������
  ������������FreeRTOS����ʼ���������
*/
int main(void)
{	
  // ������Ӳ����ʼ��
  BSP_Init();
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS-��̬����������!\r\n");

  while(1);   // ��������ִ�е�����    
}

// Ϊ�˷���������е����񴴽����������������������
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL();  // �����ٽ���
  // ����LED_Task����
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t)LED_Task,     // ������
															        (const char*)"LED_Task",      // ��������
															        (uint32_t)128,					      // �����ջ��С
															        (void*)NULL,				          // ���ݸ��������Ĳ���
															        (UBaseType_t)4, 				      // �������ȼ�
															        (StackType_t*)LED_Task_Stack, // �����ջ
															        (StaticTask_t*)&LED_Task_TCB);// ������ƿ�   

	if(NULL != LED_Task_Handle) // �����ɹ�
		printf("LED_Task���񴴽��ɹ�!\n");
	else
		printf("LED_Task���񴴽�ʧ��!\n");
  vTaskDelete(AppTaskCreate_Handle); // ɾ��AppTaskCreate����
  taskEXIT_CRITICAL(); // �˳��ٽ���
}

// LED_Task��������
static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        vTaskDelay(500);   // ��ʱ500��tick 
        printf("LED_Task Running,LED1_ON\r\n");
        LED1_OFF;     
        vTaskDelay(500);   // ��ʱ500��tick		 		
        printf("LED_Task Running,LED1_OFF\r\n");
    }
}

// �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
	// LED ��ʼ�� 
	LED_GPIO_Config();
	// ���ڳ�ʼ��	
	USART_Config();
}
 
// ��ȡ��������������ջ��������ƿ��ڴ�
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,  // ������ƿ��ڴ� 
								                   StackType_t **ppxIdleTaskStackBuffer, // �����ջ�ڴ� 
								                   uint32_t *pulIdleTaskStackSize)       // �����ջ��С
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;    // ������ƿ��ڴ� 
	*ppxIdleTaskStackBuffer=Idle_Task_Stack; // �����ջ�ڴ� 
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE; // �����ջ��С
}

// ��ȡ��ʱ������������ջ��������ƿ��ڴ�
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,  // ��ʱ��������ƿ��ڴ� 
									                  StackType_t **ppxTimerTaskStackBuffer, // ��ʱ�������ջ�ڴ� 
									                  uint32_t *pulTimerTaskStackSize)       // ��ʱ�������ջ��С
{
	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;   // ������ƿ��ڴ� 
	*ppxTimerTaskStackBuffer=Timer_Task_Stack;// �����ջ�ڴ� 
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;// �����ջ��С
}
