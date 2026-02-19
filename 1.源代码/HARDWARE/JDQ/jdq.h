#ifndef __JDQ_H
#define	__JDQ_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	PTC加热片模块h文件                   
 * 版本			: V1.0
 * 日期			: 2025.2.5
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


/***************根据自己需求更改****************/
// 继电器模块 GPIO宏定义

#define	JDQ_CLK							RCC_APB2Periph_GPIOB

#define JDQ_GPIO_PIN 				GPIO_Pin_3

#define JDQ_GPIO_PORT 				GPIOB

#define JDQ_ON 		GPIO_SetBits(JDQ_GPIO_PORT,JDQ_GPIO_PIN)
#define JDQ_OFF 	GPIO_ResetBits(JDQ_GPIO_PORT,JDQ_GPIO_PIN)

/*********************END**********************/

void JDQ_Init(void);

#endif



