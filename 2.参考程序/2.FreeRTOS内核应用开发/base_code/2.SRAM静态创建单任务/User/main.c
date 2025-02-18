#include "FreeRTOS.h"
#include "task.h"
#include "bsp_led.h"
#include "bsp_usart.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */

// 创建任务句柄
static TaskHandle_t AppTaskCreate_Handle;
// LED任务句柄 
static TaskHandle_t LED_Task_Handle;		

// AppTaskCreate任务任务堆栈
static StackType_t AppTaskCreate_Stack[128];
// LED任务堆栈 
static StackType_t LED_Task_Stack[128];
// AppTaskCreate 任务控制块
static StaticTask_t AppTaskCreate_TCB;
// AppTaskCreate 任务控制块 
static StaticTask_t LED_Task_TCB;
// 空闲任务任务堆栈 
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
// 定时器任务堆栈 
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
// 空闲任务控制块 
static StaticTask_t Idle_Task_TCB;	
// 定时器任务控制块 
static StaticTask_t Timer_Task_TCB;
 

static void AppTaskCreate(void); // 用于创建任务 
static void LED_Task(void* pvParameters); // LED_Task任务实现 
static void BSP_Init(void); // 用于初始化板载相关资源 

/**
	* 使用了静态分配内存，以下这两个函数是由用户实现，函数在task.c文件中有引用
	*	当且仅当 configSUPPORT_STATIC_ALLOCATION 这个宏定义为 1 的时候才有效
	*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
																		StackType_t **ppxTimerTaskStackBuffer, 
																		uint32_t *pulTimerTaskStackSize); // 获取定时器任务的任务堆栈和任务控制块内存

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize); // 获取空闲任务的任务堆栈和任务控制块内存
/*
  第一步：开发板硬件初始化 
  第二步：创建APP应用任务
  第三步：启动FreeRTOS，开始多任务调度
*/
int main(void)
{	
  // 开发板硬件初始化
  BSP_Init();
  printf("这是一个[野火]-STM32全系列开发板-FreeRTOS-静态创建单任务!\r\n");

  while(1);   // 正常不会执行到这里    
}

// 为了方便管理，所有的任务创建函数都放在这个函数里面
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL();  // 进入临界区
  // 创建LED_Task任务
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t)LED_Task,     // 任务函数
															        (const char*)"LED_Task",      // 任务名称
															        (uint32_t)128,					      // 任务堆栈大小
															        (void*)NULL,				          // 传递给任务函数的参数
															        (UBaseType_t)4, 				      // 任务优先级
															        (StackType_t*)LED_Task_Stack, // 任务堆栈
															        (StaticTask_t*)&LED_Task_TCB);// 任务控制块   

	if(NULL != LED_Task_Handle) // 创建成功
		printf("LED_Task任务创建成功!\n");
	else
		printf("LED_Task任务创建失败!\n");
  vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
  taskEXIT_CRITICAL(); // 退出临界区
}

// LED_Task任务主体
static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        vTaskDelay(500);   // 延时500个tick 
        printf("LED_Task Running,LED1_ON\r\n");
        LED1_OFF;     
        vTaskDelay(500);   // 延时500个tick		 		
        printf("LED_Task Running,LED1_OFF\r\n");
    }
}

// 板级外设初始化，所有板子上的初始化均可放在这个函数里面
static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
	// LED 初始化 
	LED_GPIO_Config();
	// 串口初始化	
	USART_Config();
}
 
// 获取空闲任务的任务堆栈和任务控制块内存
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,  // 任务控制块内存 
								                   StackType_t **ppxIdleTaskStackBuffer, // 任务堆栈内存 
								                   uint32_t *pulIdleTaskStackSize)       // 任务堆栈大小
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;    // 任务控制块内存 
	*ppxIdleTaskStackBuffer=Idle_Task_Stack; // 任务堆栈内存 
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE; // 任务堆栈大小
}

// 获取定时器任务的任务堆栈和任务控制块内存
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,  // 定时器任务控制块内存 
									                  StackType_t **ppxTimerTaskStackBuffer, // 定时器任务堆栈内存 
									                  uint32_t *pulTimerTaskStackSize)       // 定时器任务堆栈大小
{
	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;   // 任务控制块内存 
	*ppxTimerTaskStackBuffer=Timer_Task_Stack;// 任务堆栈内存 
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;// 任务堆栈大小
}
