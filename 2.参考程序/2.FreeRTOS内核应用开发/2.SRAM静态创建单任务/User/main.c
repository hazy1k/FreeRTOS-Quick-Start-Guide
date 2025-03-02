#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "led.h"

/* ������ */
static TaskHandle_t AppTaskCreate_Handler; // ����������
static TaskHandle_t LED_Task_Handler; // LED������

/* ȫ�ֱ������� */
static StackType_t AppTaskCreate_Stack[128]; // ������������ջ
static StackType_t LED_Task_Stack[128]; // LED����ջ
static StaticTask_t AppTaskCreate_TCB; // ����������ƿ�
static StaticTask_t LED_Task_TCB; // LED������ƿ�
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE]; // ��������ջ
static StackType_t Timer_Task_Stack[configMINIMAL_STACK_SIZE]; // ��ʱ������ջ
static StaticTask_t Idle_Task_TCB; // ����������ƿ�
static StaticTask_t Timer_Task_TCB; // ��ʱ��������ƿ�

/* ���������� */
static void AppTaskCreate(void); // ����������
static void LED_Task(void* parameter); // LED������
static void BSP_Init(void); // �弶��ʼ������

/*
    ʹ���˾�̬�����ڴ棬�������������������û�ʵ�֣������� task.c �ļ���������
    ���ҽ��� configSUPPORT_STATIC_ALLOCATION ����궨��Ϊ 1 ��ʱ�����Ч
*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

// ��������������
/*
    1.BSP��ʼ��
    2.����APP����
    3.����FreeRTOS,��������
*/
int main(void)
{
    BSP_Init(); // �弶��ʼ��
    printf("FreeRTOS SRAM Static Create Single Task Example\r\n");
    // ����AppTaskCreate����
    AppTaskCreate_Handler = xTaskCreateStatic((TaskFunction_t)AppTaskCreate,   // ������
                                             (const char*)"AppTaskCreate",     // ��������
                                             (uint32_t)128,                    // �����ջ��С
                                             (void*)NULL,                      // ���ݸ��������Ĳ���
                                             (UBaseType_t)3,                   // �������ȼ�
                                             (StackType_t*)AppTaskCreate_Stack,// ����ջ
                                             (StaticTask_t*)&AppTaskCreate_TCB);// ������ƿ�
    if(AppTaskCreate_Handler != NULL) // �����ɹ�
    {
        vTaskStartScheduler(); // ��������
    }
    while(1);
}

// Ϊ�˷�������������񴴽���������AppTaskCreate��ʵ��
static void AppTaskCreate(void)
{
    taskENTER_CRITICAL(); // �����ٽ���
    // ����LED����
    LED_Task_Handler = xTaskCreateStatic((TaskFunction_t)LED_Task,    // ������
                                        (const char*)"LED_Task",      // ��������
                                        (uint32_t)128,                // �����ջ��С
                                        (void*)NULL,                  // ���ݸ��������Ĳ���
                                        (UBaseType_t)4,               // �������ȼ�
                                        (StackType_t*)LED_Task_Stack, // ����ջ
                                        (StaticTask_t*)&LED_Task_TCB);// ������ƿ�
    if(LED_Task_Handler != NULL) // LED���񴴽��ɹ�
    {
        printf("LED Task Create Success\r\n");
    }
    else
    {
        printf("LED Task Create Failed\r\n");
    }  
    vTaskDelete(AppTaskCreate_Handler); // ɾ��AppTaskCreate������ΪLED�����Ѿ������ɹ�
    taskEXIT_CRITICAL(); // �˳��ٽ���                                 
}

// LED������
static void LED_Task(void* parameter)
{
    while(1)
    {
        LED1_ON();
        printf("LED ON\r\n");
        vTaskDelay(500);
        LED1_OFF();
        printf("LED OFF\r\n");
        vTaskDelay(500);
    }
}

// �弶��ʼ������
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �����ж����ȼ�����4
    LED_Init();
    USART_Config();
}

// ��ȡ��������������ջ��������ƿ��ڴ�
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,StackType_t **ppxIdleTaskStackBuffer,uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer=&Idle_Task_TCB;   /* ������ƿ��ڴ� */
    *ppxIdleTaskStackBuffer=Idle_Task_Stack; /* �����ջ�ڴ� */
    *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE; /* �����ջ��С */
}

// ��ȡ��ʱ������������ջ��������ƿ��ڴ�
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,StackType_t **ppxTimerTaskStackBuffer,uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer=&Timer_Task_TCB; /* ������ƿ��ڴ� */
    *ppxTimerTaskStackBuffer=Timer_Task_Stack; /* �����ջ�ڴ� */
    *pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH; /* �����ջ��С */
}
