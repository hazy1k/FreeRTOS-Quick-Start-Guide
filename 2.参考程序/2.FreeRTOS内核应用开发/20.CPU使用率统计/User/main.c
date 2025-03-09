/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "TiMbase.h"
#include "string.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t LED1_Task_Handle = NULL; // LED1任务句柄
static TaskHandle_t LED2_Task_Handle = NULL; // LED2任务句柄
static TaskHandle_t CPU_Task_Handle = NULL; // CPU使用率任务句柄

/* 内核对象句柄 */
/* 全局变量声明 */
/* 宏定义 */

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void LED1_Task(void* pvParameters);
static void LED2_Task(void* pvParameters);
static void CPU_Task(void* pvParameters);

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
    printf("FreeRTOS CPU使用率统计Demo\r\n");
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
  BaseType_t xReturn = pdPASS;
  taskENTER_CRITICAL(); // 进入临界区

  xReturn = xTaskCreate((TaskFunction_t)LED1_Task,
                        (const char*)"LED1_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)2,
                        (TaskHandle_t*)&LED1_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("LED1任务创建成功。\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t)LED2_Task,
                        (const char*)"LED2_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&LED2_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("LED2任务创建成功。\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t)CPU_Task,
                        (const char*)"CPU_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)4,
                        (TaskHandle_t*)&CPU_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("CPU使用率任务创建成功。\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
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
  uint8_t CPU_RunInfo[400]; // 保存任务运行时间信息
  while (1) 
  {
    memset(CPU_RunInfo,0,400); // 信息缓冲区清零
    vTaskList((char*)&CPU_RunInfo); // 获取任务运行时间信息
    printf("---------------------------------------------\r\n");
    printf("任务名 任务状态 优先级 剩余栈 任务序号\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    memset(CPU_RunInfo,0,400); //信息缓冲区清零
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    printf("任务名 运行计数 使用率\r\n"); 
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(500);
  }
}

// 板级初始化函数
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置中断优先级分组4
    LED_Init();
    USART_Config();
    BASIC_TIM_Init();
}
