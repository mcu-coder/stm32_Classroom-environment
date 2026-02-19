#include "pm25.h"
#include "adcx.h"
#include "delay.h"


/****
*******DHT11初始化函数
*******返回1:初始化失败
*******返回0:初始化成功
*****/
void PM25_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(PM25_LED_GPIO_CLK_ENABLE, ENABLE);	//使能端口时钟
	GPIO_InitStructure.GPIO_Pin = PM25_LED_PIN;				 				//引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO口速度为50MHz
	GPIO_Init(PM25_LED_PORT, &GPIO_InitStructure);					 	//根据设定参数初始化
	GPIO_SetBits(PM25_LED_PORT,PM25_LED_PIN);						 			//IO口输出高
}

/****
*******获取一次PM2.5值函数
*******返回值：PM2.5值
*******注意：本函数需要调用adc获取函数
*****/
float GetGP2Y(void)
{
	uint32_t AD_PM;  
	double pm; 
	PM25_LED = 0;
	delay_us(280);
	AD_PM = ADC_GetValue(ADC_Channel_5,ADC_SampleTime_239Cycles5);
	delay_us(40);
	PM25_LED = 1;
	delay_us(9680);
	if(AD_PM > 30)
		pm = ((AD_PM/4096.0)*3.3*0.17-0.1)*1000;		//计算PM2.5值
	else
		pm = 0;
	return pm;
} 

/****
*******获取平均times次PM2.5值函数
*******传入值：取多少次
*******返回值：平均的PM2.5值
*******注意：本函数需要调用adc获取函数
*****/
uint16_t Pm25_Get_Value(uint8_t times)
{
	uint32_t pm25_val=0;
	uint8_t t;
	for(t=0; t<times; t++)
	{
		pm25_val+=GetGP2Y();
		delay_ms(5);
	}
	return pm25_val/times;
} 

