/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED任务句柄 */
static TaskHandle_t LED_Task_Handle = NULL;

static void AppTaskCreate(void);
static void LED_Task(void* pvParameters);
static void BSP_Init(void);

int main(void)
{	
  BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS 

  // 开发板硬件初始化
  BSP_Init();
  printf("这是一个[野火]-STM32全系列开发板-FreeRTOS-动态创建任务!\r\n");
  // 创建AppTaskCreate任务
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,// 任务入口函数 
                        (const char*)"AppTaskCreate", // 任务名字 
                        (uint16_t)512,                // 任务栈大小
                        (void*)NULL,                  // 任务入口函数参数
                        (UBaseType_t)1,               // 任务的优先级 
                        (TaskHandle_t*)&AppTaskCreate_Handle);// 任务控制块指针 
  // 启动任务调度           
  if(pdPASS == xReturn)
    vTaskStartScheduler(); // 启动任务，开启调度
  else
    return -1;  
  while(1);   // 正常不会执行到这里    
}

// 为了方便管理，所有的任务创建函数都放在这个函数里面
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
  taskENTER_CRITICAL(); // 进入临界区
  // 创建LED_Task任务
  xReturn = xTaskCreate((TaskFunction_t)LED_Task, // 任务入口函数 
                        (const char*)"LED_Task",  // 任务名字 
                        (uint16_t)512,            // 任务栈大小 
                        (void*)NULL,	            // 任务入口函数参数 
                        (UBaseType_t)2,	          // 任务的优先级 
                        (TaskHandle_t*)&LED_Task_Handle);// 任务控制块指针
  if(pdPASS == xReturn)
    printf("创建LED_Task任务成功!\r\n");  
  vTaskDelete(AppTaskCreate_Handle);// 删除AppTaskCreate任务
  taskEXIT_CRITICAL();              // 退出临界区
}

// LED任务主体
static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        vTaskDelay(500); // 延时500个tick 
        printf("LED_Task Running,LED1_ON\r\n");
        
        LED1_OFF;     
        vTaskDelay(500); // 延时500个tick		 		
        printf("LED_Task Running,LED1_OFF\r\n");
    }
}

static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );	
	// LED 初始化 
	LED_GPIO_Config();
	// 串口初始化	
	USART_Config();
}
