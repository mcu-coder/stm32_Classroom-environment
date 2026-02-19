#ifndef __HW_H
#define	__HW_H
#include "stm32f10x.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"




/***************根据自己需求更改****************/
// HW GPIO宏定义

#define		HW_GPIO_CLK								RCC_APB2Periph_GPIOA
#define 	HW_GPIO_PORT							GPIOA
#define 	HW_GPIO_PIN								GPIO_Pin_0			

/*********************END**********************/


void HW_Init(void);
uint16_t HW_GetData(void);

#endif /* __ADC_H */

