#ifndef __LED_SEG_DISPLAY_H__
#define __LED_SEG_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define LED_SEG_REFRESH_PERIOD		1

extern __IO uint8_t dig_array[4];
void LEDSEG_Display_Init(void);

void select_digit(uint8_t dig);
void write_segment(uint8_t seg);
void DisplayDigitUINT32(uint32_t num);
void DisplayDigitINT32(int32_t num);
void DisplayDigitFLOAT(float num);
void DisplayCallback(void);
void DisplayScrollingNumber(void);  // 滚动显示学号函数
void DisplayCycleNumbers(void);     // 循环显示1-9函数
void DisplayFirstDigitOnly(uint8_t digit);  // 只显示第一位数字
void DisplaySinglePosition(uint8_t position, uint8_t digit);  // 显示指定位置的数字
void DisplayScrolling2352662(void);  // 滚动显示2352662


#ifdef __cplusplus
}
#endif
#endif /*__LED_SEG_DISPLAY_H__ */
