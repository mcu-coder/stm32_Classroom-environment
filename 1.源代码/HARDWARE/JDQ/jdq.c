#include "jdq.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	PTC加热片模块c文件                   
 * 版本			: V1.0
 * 日期			: 2025.2.5
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


void JDQ_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(JDQ_CLK, ENABLE ); //配置时钟
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	GPIO_InitStructure.GPIO_Pin = JDQ_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(JDQ_GPIO_PORT,&GPIO_InitStructure);
	

	JDQ_OFF;
}

