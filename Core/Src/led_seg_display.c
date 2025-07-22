// 原代码来自蒋老师g030_display工程
// 已经为搬迁到LED工程作出引脚修改


#include "led_seg_display.h"
#include <stdlib.h>
#include <stdio.h>
#include "gpio.h"


__IO uint8_t dig_array[4];


const uint8_t seg_table[16] = {
	0x3f,		//"0"
	0x06,		//"1"
	0x5b,		//"2"
	0x4f,		//"3"
	0x66,		//"4"
	0x6d,		//"5"
	0x7d,		//"6"
	0x07,		//"7"
	0x7f,		//"8"
	0x6f,		//"9"
	0x77,		//"A"
	0x7c,		//"b"
	0x39,		//"C"
	0x5e,		//"d"
	0x79,		//"E"
	0x71		//"F"
};

void select_digit(uint8_t dig);
void write_segment(uint8_t seg);
//void display_ascii_str(char* p);
	

//正常显示模式、负数模式、小数模式
void DisplayCallback(void)
{
	int Callback_flag =1;
	uint8_t dig;
	uint32_t tick = HAL_GetTick();
	if((tick%LED_SEG_REFRESH_PERIOD) == 0)
	{
		dig = (tick/LED_SEG_REFRESH_PERIOD)%4;
		
		// 先关闭所有位选（低电平关闭）和段（高电平关闭）
		HAL_GPIO_WritePin(GPIOA, LED_CC1_Pin|LED_CC2_Pin|LED_CC3_Pin|LED_CC4_Pin, GPIO_PIN_RESET);
		write_segment(0x00);
		
		// 检查该位是否需要显示
		if(dig_array[dig] != 0xff)		//该位需要显示
		{
			// 选择要显示的位
			select_digit(dig);
			
			// 显示对应内容
			if((dig_array[dig]&0x80) != 0)		//display minus
			{
				write_segment(0x40);	
			}
			else if((dig_array[dig]&0x10) != 0)		//add dot
			{
				write_segment(seg_table[dig_array[dig]&0x0f] | 0x80);
			}
			else
			{
				write_segment(seg_table[dig_array[dig]&0x0f]);
			}
		}
		// 如果dig_array[dig] == 0xff，该位保持关闭状态，不进行位选
	}
	Callback_flag =0;
	


}

//这个是选择点亮哪一位digit（主要debug在了这里，改变位选逻辑）
void select_digit(uint8_t dig)
{
	switch(dig)
	{
		case 3:
			HAL_GPIO_WritePin(GPIOA, LED_CC1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, LED_CC2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC4_Pin, GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOA, LED_CC1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, LED_CC3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC4_Pin, GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOA, LED_CC1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, LED_CC4_Pin, GPIO_PIN_RESET);
			break;
		case 0:
			HAL_GPIO_WritePin(GPIOA, LED_CC1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_CC4_Pin, GPIO_PIN_SET);
			break;
		default:
			break;
	}
}

//这个是选择点亮哪一个笔画
void write_segment(uint8_t seg)
{
	// 段信号：高电平点亮
	// 先清空所有段 (PB0-PB7设为低电平)
	HAL_GPIO_WritePin(GPIOB, 0xFF, GPIO_PIN_RESET);
	// 再设置要点亮的段为高电平
	HAL_GPIO_WritePin(GPIOB, seg, GPIO_PIN_SET);
}


//这个是选择亮起什么数字
void DisplayDigitUINT32(uint32_t num)
{
	uint32_t tmp;
	if(num > 9999)
	{
		for(uint8_t i=0;i<4;i++)
			dig_array[i] = 0x0f;
	}

	//主逻辑——直接给dig_array赋值
	else
	{
		tmp = num;
		for(uint8_t i=0;i<4;i++)
		{
			if((tmp == 0)&&(i!=0))
			{
				dig_array[i] = 0xff;
			}
			else
			{
				dig_array[i] = tmp%10;			
			}
			tmp = tmp/10;
		}
			
	}
}

void DisplayDigitINT32(int32_t num)
{
	int32_t tmp;
	if(num > 9999)
	{
		for(uint8_t i=0;i<4;i++)
			dig_array[i] = 0x0f;
	}
	else if(num < -999)
	{
		for(uint8_t i=0;i<4;i++)
			dig_array[i] = 0x0f;	
		dig_array[3] = 0x80;
	}
	else
	{
		tmp = num;
		for(uint8_t i=0;i<4;i++)
		{
			if((tmp == 0)&&(i!=0))
			{
				dig_array[i] = 0xff;
			}
			else
			{
				dig_array[i] = abs(tmp)%10;			
			}
			tmp = tmp/10;
		}
		if(num <0)
		{
			for(uint8_t i=0;i<4;i++)
			{
				if(dig_array[i] == 0xff)
				{
					dig_array[i] = 0x80;
					break;
				}
			}
		}
			
	}
}

// 只显示最右边位的数字
void DisplayFirstDigitOnly(uint8_t digit)
{
	// 从现象看：最左边(dig_array[3])不亮=正确关闭，其他位都亮=没有正确关闭
	// 所以dig_array[0]应该是最右边
	dig_array[0] = (digit <= 9) ? digit : 0xff;  // 最右边显示数字
	dig_array[1] = 0xff;  // 必须关闭
	dig_array[2] = 0xff;  // 必须关闭
	dig_array[3] = 0xff;  // 最左边关闭（这个是对的）
}

// 测试函数：显示指定位置的数字
void DisplaySinglePosition(uint8_t position, uint8_t digit)
{
	// 清空所有位
	for(uint8_t i=0;i<4;i++)
	{
		dig_array[i] = 0xff;  // 所有位关闭
	}
	
	// 只在指定位置显示数字
	if(position < 4 && digit <= 9)
	{
		dig_array[position] = digit;
	}
}

// 滚动显示2352662 - 从右边进入效果
void DisplayScrolling2352662(void)
{
	static uint8_t scroll_data[] = {2, 3, 5, 2, 6, 6, 2};  // 要滚动的数字序列
	static uint8_t step = 0;  // 当前步骤
	static uint32_t last_update_time = 0;  // 上次更新时间
	uint32_t current_time = HAL_GetTick();
	
	// 每500ms更新一次
	if(current_time - last_update_time >= 500)
	{
		last_update_time = current_time;
		
		// 清空显示数组
		for(uint8_t i = 0; i < 4; i++)
		{
			dig_array[i] = 0xff;  // 先全部关闭
		}
		
		if(step == 0)  // 第1步：   2 
		{
			dig_array[0] = scroll_data[0];  
		}
		else if(step == 1)  // 第2步：  32 
		{
			dig_array[0] = scroll_data[1];  
			dig_array[1] = scroll_data[0];  
		}
		else if(step == 2)  // 第3步： 532 
		{
			dig_array[0] = scroll_data[2];  
			dig_array[1] = scroll_data[1];  
			dig_array[2] = scroll_data[0];  
		}
		else if(step == 3)  // 第4步：2532 
		{
			dig_array[0] = scroll_data[3];  
			dig_array[1] = scroll_data[2];  
			dig_array[2] = scroll_data[1];  
			dig_array[3] = scroll_data[0];  
		}
		else if(step <= 6)  // 继续滚动阶段
		{
			uint8_t offset = step - 3;
			for(uint8_t i = 0; i < 4; i++)
			{
				uint8_t data_index = (offset + i) % 7;
				dig_array[3-i] = scroll_data[data_index];
			}
		}
		else if(step == 7)  // 退出第1步：6622 -> 622 
		{
			dig_array[1] = scroll_data[6];
			dig_array[2] = scroll_data[5];
			dig_array[3] = scroll_data[4];
		}
		else if(step == 8)  // 退出第2步：622 -> 22 
		{
			dig_array[2] = scroll_data[6];
			dig_array[3] = scroll_data[4];
		}
		else if(step == 9)  // 退出第3步：22 -> 2 
		{
			dig_array[3] = scroll_data[6];
		}
		// step == 10时全部清空
		
		// 更新步骤
		step++;
		if(step >= 4 + 3 + 4)  // 进入(4步) + 滚动(3步) + 退出(4步)
		{
			// step = 0;
			return;
		}
	}
}


