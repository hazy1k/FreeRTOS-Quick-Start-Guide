/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TimerHandle_t Swtmr1_Handle = NULL; // 软件定时器1句柄
static TimerHandle_t Swtmr2_Handle = NULL; // 软件定时器2句柄

/* 内核对象句柄 */
/* 全局变量声明 */
static uint32_t TmrCb_Count1 = 0; // 记录软件定时器1回调函数执行次数
static uint32_t TmrCb_Count2 = 0; // 记录软件定时器2回调函数执行次数
/* 宏定义 */


/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void Swtmr1_Callback(void* pvParameters); 
static void Swtmr2_Callback(void* pvParameters); 

static void BSP_Init(void); // 板级初始化函数

// 主函数启动流程
/*
    1.BSP初始化
    2.创建APP任务
    3.启动FreeRTOS,开启调度
*/
int main(void)
{
    BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
    BSP_Init(); // 板级初始化
    printf("FreeRTOS软件定时器实验\r\n");
    // 创建AppTaskCreate任务   
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*)"AppTaskCreate",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)1, 
                        (TaskHandle_t*)&AppTaskCreate_Handle); 
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // 启动FreeRTOS
    }
    else
    {
        return -1;
    }
    while(1);
}

// 为了方便管理，所有任务创建函数都在AppTaskCreate中实现
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL(); // 进入临界区

  Swtmr1_Handle = xTimerCreate((const char*)"AutoReloadTimer", // 定时器任务名称
                            (TickType_t)1000, // 定时器周期1000(tick)
                            (UBaseType_t)pdTRUE, // 周期模式
                            (void*)1, // 每个计时器分配一个索引的唯一ID
                            (TimerCallbackFunction_t)Swtmr1_Callback); // 定时器回调函数
  if(Swtmr1_Handle != NULL)
  {
    xTimerStart(Swtmr1_Handle, 0); // 启动定时器(周期模式)
  }
  Swtmr2_Handle = xTimerCreate((const char*)"OneShotTimer", // 定时器任务名称
                            (TickType_t)500, // 定时器周期5000(tick)
                            (UBaseType_t)pdFALSE, // 单次模式
                            (void*)2, // 每个计时器分配一个索引的唯一ID
                            (TimerCallbackFunction_t)Swtmr2_Callback); // 定时器回调函数
  if(Swtmr2_Handle != NULL)
  {
    xTimerStart(Swtmr2_Handle, 0); // 启动定时器(单次模式)
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
}


// 周期定时器1回调函数
static void Swtmr1_Callback(void* pvParameters)
{
  TickType_t tick_num1;
  TmrCb_Count1++; // 记录软件定时器1回调函数执行次数
  tick_num1 = xTaskGetTickCount(); // 获取滴答定时器的计数值
  LED1_TOGGLE();
  printf("软件定时器1回调函数执行次数：%d，滴答定时器计数值：%d\r\n", TmrCb_Count1, tick_num1);
}

// 单次定时器2回调函数
static void Swtmr2_Callback(void* pvParameters)
{
  TickType_t tick_num2;
  TmrCb_Count2++;
  tick_num2 = xTaskGetTickCount(); // 获取滴答定时器的计数值
  printf("软件定时器2回调函数执行次数：%d，滴答定时器计数值：%d\r\n", TmrCb_Count2, tick_num2);
}

// 板级初始化函数
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置中断优先级分组4
    LED_Init();
    USART_Config();
    Key_GPIO_Config();
}
