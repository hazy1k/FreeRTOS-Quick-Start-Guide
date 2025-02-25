#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "uart.h"

int main(void)
{	
	LED_Init(); // LED接口初始化
	LED1_ON();
}
