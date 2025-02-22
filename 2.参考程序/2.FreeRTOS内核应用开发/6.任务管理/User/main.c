#include "FreeRTOS.h"
#include "task.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;// 创建任务句柄 
static TaskHandle_t LED_Task_Handle = NULL;// LED任务句柄 
static TaskHandle_t KEY_Task_Handle = NULL;// KEY任务句柄 
static void AppTaskCreate(void); // 用于创建任务
static void LED_Task(void* pvParameters);// LED_Task任务实现 
static void KEY_Task(void* pvParameters);// KEY_Task任务实现 
static void BSP_Init(void); // 用于初始化板载相关资源 

int main(void)
{	
  BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
  // 开发板硬件初始化
  BSP_Init();
  printf("这是一个[野火]-STM32全系列开发板-FreeRTOS任务管理实验！\n\n");
  printf("按下KEY1挂起任务，按下KEY2恢复任务\n");
  // 创建AppTaskCreate任务
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate, // 任务入口函数 
                        (const char*)"AppTaskCreate",  // 任务名字 
                        (uint16_t)512,  // 任务栈大小 
                        (void*)NULL,    // 任务入口函数参数 
                        (UBaseType_t)1, // 任务的优先级 
                        (TaskHandle_t*)&AppTaskCreate_Handle);// 任务控制块指针  
  // 启动任务调度          
  if(pdPASS == xReturn)
    vTaskStartScheduler(); // 启动任务，开启调度
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}

// 为了方便管理，所有的任务创建函数都放在这个函数里面
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
  taskENTER_CRITICAL();        // 进入临界区
  // 创建LED_Task任务
  xReturn = xTaskCreate((TaskFunction_t)LED_Task, // 任务入口函数 
                        (const char*)"LED_Task",  // 任务名字 
                        (uint16_t)512,            // 任务栈大小
                        (void*)NULL,	            // 任务入口函数参数 
                        (UBaseType_t)2,	          // 任务的优先级 
                        (TaskHandle_t*)&LED_Task_Handle);// 任务控制块指针 
  if(pdPASS == xReturn)
    printf("创建LED_Task任务成功!\r\n");
  // 创建KEY_Task任务
  xReturn = xTaskCreate((TaskFunction_t)KEY_Task, // 任务入口函数
                        (const char*)"KEY_Task",  // 任务名字 
                        (uint16_t)512,            // 任务栈大小 
                        (void*)NULL,              // 任务入口函数参数 
                        (UBaseType_t)3,           // 任务的优先级 
                        (TaskHandle_t*)&KEY_Task_Handle);// 任务控制块指针  
  if(pdPASS == xReturn)
    printf("创建KEY_Task任务成功!\r\n");
  vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
  taskEXIT_CRITICAL();            //退出临界区
}

// LED_Task任务实现
static void LED_Task(void* parameter)
{	
  while (1)
  {
    LED1_ON;
    printf("LED_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   // 延时500个tick
    LED1_OFF;     
    printf("LED_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);  // 延时500个tick 
  }
}

// KEY_Task任务实现
static void KEY_Task(void* parameter)
{	
  while (1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
    { 
      // K1 被按下
      printf("挂起LED任务！\n");
      vTaskSuspend(LED_Task_Handle); // 挂起LED任务
      printf("挂起LED任务成功！\n");
    } 
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
    { 
      // K2 被按下
      printf("恢复LED任务！\n");
      vTaskResume(LED_Task_Handle); // 恢复LED任务！
      printf("恢复LED任务成功！\n");
    }
    vTaskDelay(20); // 延时20个tick
  }
}

// 开发板硬件初始化
static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	// LED 初始化 
	LED_GPIO_Config();
	// 串口初始化	
	USART_Config();
  // 按键初始化
  Key_GPIO_Config();
}
