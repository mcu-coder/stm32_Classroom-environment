#include "stm32f10x.h"
#include "led.h"
#include "beep.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "dht11.h"
#include "LDR.h"
#include "oled.h"
#include "key.h"
#include "Modules.h"
#include "TIM2.h"
#include "TIM3.h"
#include "TIM4.h"
#include "adcx.h"
#include "flash.h"
#include "JDQ.h"
#include "stepmotor.h"
#include "fan.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "stm32f10x_iwdg.h"

 

#define KEY_Long1	11

#define KEY_1	1
#define KEY_2	2
#define KEY_3	3
#define KEY_4	4

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

//传感器变量声明
u8 temp;//温度变量
u8 humi;//湿度变量
u16 lux;//光照值变量
u16 soil_Humi;//土壤湿度变量
u16 CO2;//CO2变量


SensorModules sensorData;								//声明传感器数据结构体变量
SensorThresholdValue Sensorthreshold;		//声明传感器阈值结构体变量
DriveModules driveData;									//声明驱动器状态结构体变量

int mode = 0;	//系统模式  1自动  2手动  3设置

//系统静态变量
static uint8_t count_a = 1;  //自动模式按键数
 uint8_t count_m = 1;  //手动模式按键数
static uint8_t count_s = 1;	 //设置模式按键数
//static uint8_t LED_Flag = 0;
//static uint8_t Fan_Flag = 0;
//static uint8_t Bump_Flag = 0;
//static uint8_t Beep_Flag = 0;
//static uint8_t Curtain_Flag = 0;
//static uint8_t NOW_Curtain_Flag = 0;

extern volatile uint8_t motor_busy;  // 电机忙标志
/**
  * @brief  显示菜单内容
  * @param  无
  * @retval 无
  */
enum 
{
	AUTO_MODE = 0,
	MANUAL_MODE,
	SETTINGS_MODE
	
}MODE_PAGES;

/**
  * @brief  显示菜单1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_autoPage1(void)		//自动模式菜单第一页
{
	//显示“环境温度：  C”
//	OLED_ShowChinese(0,0,13,16,1); 	//环
//	OLED_ShowChinese(16,0,14,16,1);	//境
	OLED_ShowChinese(0,0,0,16,1);	//温
	OLED_ShowChinese(16,0,2,16,1);	//度
	OLED_ShowChar(32,0,':',16,1);
	
	//显示“环境湿度:   %”
//	OLED_ShowChinese(0,16,13,16,1);	//环
//	OLED_ShowChinese(16,16,14,16,1);//境
	OLED_ShowChinese(64,0,1,16,1);	//湿
	OLED_ShowChinese(80,0,2,16,1);	//度
	OLED_ShowChar(96,0,':',16,1);
	
	//显示”光照强度：  Lux
	OLED_ShowChinese(0,16,3,16,1); //光
	OLED_ShowChinese(16,16,4,16,1);//照
	OLED_ShowChinese(32,16,5,16,1);//强
	OLED_ShowChinese(48,16,2,16,1);//度
	OLED_ShowChar(64,16,':',16,1);
	
	//显示“PM2.5浓度”
	OLED_ShowString(0,32,"PM25",16,1);	
	OLED_ShowChinese(32,32,12,16,1);	//浓
	OLED_ShowChinese(48,32,2,16,1);	//度
	OLED_ShowChar(64,32,':',16,1);
	
	//显示“是否有雨”
	OLED_ShowChinese(0,48,55,16,1); //是
	OLED_ShowChinese(16,48,56,16,1);//否
	OLED_ShowChinese(32,48,53,16,1);//有
	OLED_ShowChinese(48,48,57,16,1);//雨
	OLED_ShowChar(64,48,':',16,1);
	
	OLED_Refresh();
	
}
void OLED_autoPage2(void)   //自动模式菜单第二页
{
	
	//显示“光照强度：  Lux”
	OLED_ShowChinese(0,0,3,16,1); //光
	OLED_ShowChinese(16,0,4,16,1);//照
	OLED_ShowChinese(32,0,5,16,1);//强
	OLED_ShowChinese(48,0,2,16,1);//度
	OLED_ShowChar(64,0,':',16,1);
	
	//显示“CO2浓度： ppm”
	OLED_ShowString(0,16,"CO2",16,1);
	OLED_ShowChinese(25,16,8,16,1);//浓
	OLED_ShowChinese(41,16,2,16,1);//度
	OLED_ShowChar(57,16,':',16,1);
	
	OLED_Refresh();
}

void SensorDataDisplay1(void)		//传感器数据显示第一页
{
	//显示温度数据
	OLED_ShowNum(40,0,sensorData.temp,2,16,1);
	//显示湿度数据
	OLED_ShowNum(104,0,sensorData.humi,2,16,1);
	//显示光照强度数据
	OLED_ShowNum(90, 16, sensorData.lux,4,16,1);	
	//显示pm25数据
	OLED_ShowNum(90,32,sensorData.pm25,4,16,1);
	if(sensorData.rain)
		OLED_ShowChinese(90,48,53,16,1);
	else
		OLED_ShowChinese(90,48,54,16,1);

}
void SensorDataDisplay2(void)		//传感器数据显示第二页
{

	//显示光照强度数据
	OLED_ShowNum(90,0,sensorData.lux,4,16,1);
	//显示CO2数据
//	OLED_ShowNum(90, 16, sensorData.CO2,4,16,1);	

}

/**
  * @brief  显示手动模式设置界面1
  * @param  无
  * @retval 无
  */
void OLED_manualPage1(void)
{
	
	//显示“灯光”
	OLED_ShowChinese(16,0,28,16,1);//灯
	OLED_ShowChinese(32,0,29,16,1);//光
	OLED_ShowChar(64,0,':',16,1);

	//显示“风扇”
	OLED_ShowChinese(16,16,33,16,1);//风
	OLED_ShowChinese(32,16,34,16,1);//扇
	OLED_ShowChar(64,16,':',16,1);
	
	//显示“加热”
	OLED_ShowChinese(16,32,35,16,1);//加
	OLED_ShowChinese(32,32,44,16,1);//热
	OLED_ShowChar(64,32,':',16,1);
	
	//显示“窗户”
	OLED_ShowChinese(16,48,30,16,1);//窗
	OLED_ShowChinese(32,48,32,16,1);//户
	OLED_ShowChar(64,48,':',16,1);
}



/**
  * @brief  显示手动模式设置参数界面1
  * @param  无
  * @retval 无
  */
void ManualSettingsDisplay1(void)
{
	if(driveData.LED_Flag)
	{
		OLED_ShowChinese(96,0,40,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,0,42,16,1); 	//关
	}
	
	if(driveData.Fan_Flag)
	{
		OLED_ShowChinese(96,16,40,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,16,42,16,1); 	//关
	}
	
	if(driveData.Jdq_Flag)
	{
		OLED_ShowChinese(96,32,40,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,32,42,16,1); 	//关
	}
	
	if(driveData.NOW_StepMotor_Flag)
	{
		OLED_ShowChinese(96,48,40,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,48,42,16,1); 	//关
	}
}


/**
  * @brief  显示系统阈值设置界面1
  * @param  无
  * @retval 无
  */
void OLED_settingsPage1(void)
{
	//显示“温度阈值”
	OLED_ShowChinese(16,0,0,16,1);	
	OLED_ShowChinese(32,0,2,16,1);	
	OLED_ShowChinese(48,0,50,16,1);	
	OLED_ShowChinese(64,0,52,16,1);	
	OLED_ShowChar(80,0,':',16,1);

	//显示“湿度阈值”
	OLED_ShowChinese(16,16,0,16,1);	
	OLED_ShowChinese(32,16,2,16,1);	
	OLED_ShowChinese(48,16,51,16,1);	
	OLED_ShowChinese(64,16,52,16,1);	
	OLED_ShowChar(80,16,':',16,1);
	
	//显示“光照阈值”
	OLED_ShowChinese(16,32,3,16,1);	
	OLED_ShowChinese(32,32,4,16,1);	
	OLED_ShowChinese(48,32,26,16,1);	
	OLED_ShowChinese(64,32,27,16,1);	
	OLED_ShowChar(80,32,':',16,1);
	
	//显示“PM阈值”
	OLED_ShowString(16,48,"PM25",16,1);	
	OLED_ShowChinese(48,48,26,16,1);	
	OLED_ShowChinese(64,48,27,16,1);	
	OLED_ShowChar(80,48,':',16,1);
	
}

void OLED_settingsPage2(void)
{
	//显示“光照阈值”
	OLED_ShowChinese(16,0,3,16,1);	
	OLED_ShowChinese(32,0,4,16,1);
	OLED_ShowChinese(48,0,16,16,1);	
	OLED_ShowChinese(64,0,17,16,1);	
	OLED_ShowChar(80,0,':',16,1);

	//显示“CO2阈值”
	OLED_ShowString(16,16,"CO2",16,1);
	OLED_ShowChinese(40,16,16,16,1);	
	OLED_ShowChinese(56,16,17,16,1);	
	OLED_ShowChar(72,16,':',16,1);
}

void SettingsThresholdDisplay1(void)
{
	//显示温度上限阈值数值
	OLED_ShowNum(90,0, Sensorthreshold.tempValue_Max, 2,16,1);
	//显示温度下限阈值数值
	OLED_ShowNum(90, 16, Sensorthreshold.tempValue_Min, 2,16,1);
		
	//显示光照阈值数值
	OLED_ShowNum(90, 32, Sensorthreshold.luxValue , 3,16,1);
	
	//显示PM2.5浓度阈值数值
	OLED_ShowNum(90, 48, Sensorthreshold.Pm25Value, 3,16,1);
}

void SettingsThresholdDisplay2(void)
{
	//显示光照强度阈值数值
	OLED_ShowNum(90,0, Sensorthreshold.luxValue, 3,16,1);
	//显示CO2浓度阈值数值
	//OLED_ShowNum(90, 16, Sensorthreshold.CO2Value, 4,16,1);
		
}
	
/**
  * @brief  记录自动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetAuto(void)  
{
	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_a++;
		if (count_a > 2)
		{
			count_a = 1;
		}
		OLED_Clear();
	}
	return count_a;
}

/**
  * @brief  记录手动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetManual(void)  
{

	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_m++;
		if (count_m > 4)  		//一共可以控制的外设数量
		{
			count_m = 1;
		}
	}
	return count_m;
}

/**
  * @brief  记录阈值界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetSelection(void)
{
	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_s++;
	}
	else if (count_s > 4)
		{
			OLED_Clear();
			count_s = 1;
		}
	return count_s;
}


/**
  * @brief  显示手动模式界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_manualOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 3:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,'>',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 4:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,'>',16,1);
			break;
		default: break;
	}
}

/**
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_settingsOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 3:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,'>',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 4:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,'>',16,1);
			break;
		default: break;
	}
}

/**
  * @brief  自动模式控制函数
  * @param  无
  * @retval 无
  */
void AutoControl(void)
{
	
	if(sensorData.temp<Sensorthreshold.tempValue_Min)//当前值小于阈值
		driveData.Jdq_Flag = 1;
	else
		driveData.Jdq_Flag = 0;
	if(sensorData.lux<Sensorthreshold.luxValue)//当前值小于阈值
		driveData.LED_Flag = 1;
	else
		driveData.LED_Flag = 0;
	if(sensorData.rain)
		driveData.NOW_StepMotor_Flag= 1;
	else
		driveData.NOW_StepMotor_Flag= 0;
	if(sensorData.pm25>Sensorthreshold.Pm25Value)//当前值大于阈值
	{
		driveData.Beep_Flag = 1;
	}
	else
	{
		driveData.Beep_Flag = 0;
	}
	if(sensorData.temp>Sensorthreshold.tempValue_Max ||sensorData.pm25>Sensorthreshold.Pm25Value)//当前值大于阈值
		driveData.Fan_Flag = 1;
	else
		driveData.Fan_Flag = 0;
	
}

/**
  * @brief  手动模式控制函数
  * @param  无
  * @retval 无
  */
void ManualControl(uint8_t num)
{
	switch(num)
	{
		case 1:	
			//显示外设开关
			if(KeyNum == KEY_3)
				driveData.LED_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.LED_Flag = 0;

			break;
		case 2:	
			if(KeyNum == KEY_3)
				driveData.Fan_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.Fan_Flag = 0;
		
			break;
		case 3:	
			if(KeyNum == KEY_3)
				driveData.Jdq_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.Jdq_Flag = 0;
			
			break;
		case 4:	
			if(KeyNum == KEY_3)
				driveData.NOW_StepMotor_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.NOW_StepMotor_Flag = 0;
			
		default: break;
	}

}

/**
  * @brief  控制函数
  * @param  无
  * @retval 无
  */
void Control_Manager(void)
{
	 
	if(driveData.Beep_Flag)//蜂鸣器控制
	{
		BEEP_On();
	}
	else
	{
		BEEP_Off();
	}
	 
	
}

/**
  * @brief  阈值设置函数
  * @param  无
  * @retval 无
  */
void ThresholdSettings(uint8_t num)
{
	switch (num)
	{
		case 1:
			if (KeyNum == KEY_3)//按键三按下
			{
				KeyNum = 0;
				Sensorthreshold.tempValue_Max += 1;//阈值增加
				if (Sensorthreshold.tempValue_Max > 35)//阈值上限
				{
					Sensorthreshold.tempValue = 35;
				}
			}
			else if (KeyNum == KEY_4)//按键四按下
			{
				KeyNum = 0;
				Sensorthreshold.tempValue_Max -= 1;//阈值减小
				if (Sensorthreshold.tempValue < 20)//阈值下限
				{
					Sensorthreshold.tempValue = 20;
				}				
			}
			break;
			
		case 2:
			if (KeyNum == KEY_3)//按键三按下
			{
				KeyNum = 0;
				Sensorthreshold.tempValue_Min += 1;//阈值增加
				if (Sensorthreshold.tempValue_Min > 20)//阈值上限
				{
					Sensorthreshold.tempValue_Min = 20;
				}
			}
			else if (KeyNum == KEY_4)//按键四按下
			{
				KeyNum = 0;
				Sensorthreshold.tempValue_Min -= 1;//阈值减小
				if (Sensorthreshold.tempValue_Min < 5)//阈值下限
				{
					Sensorthreshold.tempValue_Min = 5;
				}				
			}	
			break;
		case 3:
			if (KeyNum == KEY_3)//按键三按下
			{
				KeyNum = 0;
				Sensorthreshold.luxValue += 10;//阈值增加
				if (Sensorthreshold.luxValue > 400)//阈值上限
				{
					Sensorthreshold.luxValue = 400;
				}
			}
			else if (KeyNum == KEY_4)//按键四按下
			{
				KeyNum = 0;
				Sensorthreshold.luxValue -= 10;//阈值减小
				if (Sensorthreshold.luxValue < 100)//阈值下限
				{
					Sensorthreshold.luxValue = 100;
				}				
			}
			break;
		case 4:
			if (KeyNum == KEY_3)//按键三按下
			{
				KeyNum = 0;
				Sensorthreshold.Pm25Value += 10;//阈值增加
				if (Sensorthreshold.Pm25Value > 550)
				{//阈值上限
					Sensorthreshold.Pm25Value = 550;
				}
			}
			else if (KeyNum == KEY_4)//按键四按下
			{
				KeyNum = 0;
				Sensorthreshold.Pm25Value -= 10;//阈值减小
				if (Sensorthreshold.Pm25Value < 10)//阈值下限
				{
					Sensorthreshold.Pm25Value = 10;
				}				
			}
			break;
		default: break;		
	}
}
/**
  * @brief  机智云按键配网
  * @param  无
  * @retval 无
  */
void ScanGizwitsMode(void)
{
	if(!KEY3)
	{
		delay_ms(20);
		if(!KEY3)
		{
			//显示“热点配网”
	    OLED_ShowChinese(32,16,44,16,1); 	//热
	    OLED_ShowChinese(48,16,45,16,1);	//点                               
	    OLED_ShowChinese(64,16,48,16,1);	//配
	    OLED_ShowChinese(80,16,49,16,1);  //网
			gizwitsSetMode(WIFI_SOFTAP_MODE);
			OLED_Refresh();
		}
	}
	else if(!KEY4)
	{
		delay_ms(20);
		if(!KEY4)
		{
			//显示“一键配网”
	    OLED_ShowChinese(32,16,46,16,1); 	//一
	    OLED_ShowChinese(48,16,47,16,1);	//键
	    OLED_ShowChinese(64,16,48,16,1);	//配
	    OLED_ShowChinese(80,16,49,16,1);  //网
			gizwitsSetMode(WIFI_AIRLINK_MODE);
			OLED_Refresh();
		}
	}
}

void IWDG_Config(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_64);   // 40 kHz / 64 = 625 Hz
    IWDG_SetReload(625);                    // 1 秒超时
    IWDG_Enable();
}
/**
  * @brief  flash阈值读取函数
  * @param  无
  * @retval 无
  */
void FLASH_ReadThreshold()
{
	Sensorthreshold.tempValue_Max	= FLASH_R(FLASH_START_ADDR);	//从指定页的地址读FLASH
	Sensorthreshold.tempValue_Min = FLASH_R(FLASH_START_ADDR+2);	//从指定页的地址读FLASH
	Sensorthreshold.luxValue = FLASH_R(FLASH_START_ADDR+4);	//从指定页的地址读FLASH
	Sensorthreshold.Pm25Value = FLASH_R(FLASH_START_ADDR+6);	//从指定页的地址读FLASH
	if(Sensorthreshold.tempValue_Max > 50 || Sensorthreshold.tempValue_Min > 20 || Sensorthreshold.luxValue > 300|| Sensorthreshold.Pm25Value > 300)//flash读取乱码重置flash中的数值
	{
		FLASH_W(FLASH_START_ADDR,28,20,150,200);
		FLASH_ReadThreshold();//调用flash阈值读取函数
	}
}

uint8_t OLED_ClearFlag_New;
uint8_t OLED_ClearFlag_OLD;

int main(void)
{ 
  SystemInit();//配置系统时钟为72M	
	delay_init(72);
	ADCx_Init();//ADC初始化
	LED_Init();//LED初始化
	LED_On();//打开LED
 
	USART2_Config();//串口2初始化 
	delay_ms(200);
	ScanGizwitsMode();//机智云配网模式
  delay_ms(500);
	OLED_Clear();
	
  while (1)
  {
		
	
		
		SensorScan();	//获取传感器数据

		
		switch(mode)//模式选择
		{
			case AUTO_MODE://自动模式
				
 
				Control_Manager();//控制函数执行相应的执行器
				
				
				userHandle();//上报
				gizwitsHandle((dataPoint_t *)&currentDataPoint);//下发
				break;
				
				case MANUAL_MODE://手动模式
			
		
				driveData.Mode_Flag=1;
				
			 
				{
					KeyNum = 0;
					mode = AUTO_MODE;//切换至自动模式
					count_a = 1;//自动模式下按键次数赋值
					OLED_Clear();//清屏
				}
				Control_Manager();//控制函数执行相应的执行器
				
						userHandle();//上报
						gizwitsHandle((dataPoint_t *)&currentDataPoint);//下发
				break;
				
				case SETTINGS_MODE://阈值调节模式
					

					OLED_settingsOption(SetSelection());	//实现阈值设置页面的选择功能
					ThresholdSettings(SetSelection());	//实现阈值调节功能	
				 
					userHandle();//上报
					gizwitsHandle((dataPoint_t *)&currentDataPoint);//下发
				break;
				default: break;
					
		}
	 
		
		
  }
}
