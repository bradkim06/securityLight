#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "lcd_c.h"
#include "shell.h"
#include "rtc.h"
#include "metering.h"

uint8_t currentLCDMode = LCD_CURRENT_DATE;
uint8_t lightMode = 0;
extern light_t light;

// Local variable
typedef struct {
	char c;
	int value;
	uint8_t pattern;
} lcd_pattern_t;

const lcd_pattern_t lcdPattern[] = {
	{ '0', 0, 0xAF }, { '1', 1, 0x06 }, { '2', 2, 0xCB }, { '3', 3, 0x4F }, { '4', 4, 0x66 },
	{ '5', 5, 0x6D }, { '6', 6, 0xEC }, { '7', 7, 0x07 }, { '8', 8, 0xEF }, { '9', 9, 0x67 },
	{ '-', 0, 0x40 }, { ' ', 0, 0x00 }, { 'O', 0, 0xAF }, { 'F', 0, 0xE8 },
};

static uint8_t lcd_getCharPattern(char c)
{
	int num = sizeof(lcdPattern) / sizeof(lcd_pattern_t);

	// convert lower case char to upper case char.
	if (c >= 'a' && c <= 'z') {
		c -= 0x20;
	}

	uint8_t pattern = 0;
	for (int i = 0; i < num; i++) {
		if (c == lcdPattern[i].c) {
			pattern = lcdPattern[i].pattern;
			break;
		}
	}

	return pattern;
}

/*! LCD Memory map. Used to set individual LCD segments */
static uint8_t LCDMemMap[HAL_LCD_MEMMAP_SIZE] = { 0 };

void hal_lcd_updatememory(uint8_t baseposition, uint8_t num_segments)
{
	uint8_t i;

	for (i = 0; i < num_segments; i++) {
		LCD_C_setMemory(LCD_C_BASE, baseposition + i, LCDMemMap[baseposition + i]);
	}
}

void LCD_clearLCD(void)
{
	for (int i = 0; i < HAL_LCD_MEMMAP_SIZE; i++) {
		LCDMemMap[i] = 0;
	}
	lcd_updateAll();
}

void LCD_fullLCD(void)
{
	for (int i = 0; i < HAL_LCD_MEMMAP_SIZE; i++) {
		LCDMemMap[i] = 0x0F;
	}
	lcd_updateAll();
}

void DisplayDay(void)
{
	memset(LCDMemMap, 0, 16);
	char Array[4] = "";

	rtcData = RTC_C_getCalendarTime(RTC_C_BASE);

	// year
	sprintf(Array, "%04lu", (unsigned long)rtcData.Year);
	for (int i = 0; i < 4; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 0] &= 1;
		LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 1] = (value & 0x0F);
	}

	// month
	sprintf(Array, "%02lu", (unsigned long)rtcData.Month);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 8 + 0] &= 1;
		LCDMemMap[i * 2 + 8 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 8 + 1] = (value & 0x0F);
	}

	// day
	sprintf(Array, "%02lu", (unsigned long)rtcData.DayOfMonth);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 12 + 0] &= 1;
		LCDMemMap[i * 2 + 12 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 12 + 1] = (value & 0x0F);
	}

	LCDMemMap[8] |= 1;
	LCDMemMap[12] |= 1;
}

void DisplayTime(uint8_t mode)
{
	memset(LCDMemMap, 0, 16);
	char Array[2] = "";
	Time_t time;

	switch (mode) {
	case LCD_LIGHTON_TIME:
		memcpy(&time, &light.onTime, 2);
		break;
	case LCD_LIGHTOFF_TIME:
		memcpy(&time, &light.offTime, 2);
		break;
	case LCD_CURRENT_TIME:
		rtcData = RTC_C_getCalendarTime(RTC_C_BASE);
		time.hour = rtcData.Hours;
		time.minute = rtcData.Minutes;
		break;
	}

	// hour
	sprintf(Array, "%02lu", (unsigned long)time.hour);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 8 + 0] &= 1;
		LCDMemMap[i * 2 + 8 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 8 + 1] = (value & 0x0F);
	}

	// minute
	sprintf(Array, "%02lu", (unsigned long)time.minute);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 12 + 0] &= 1;
		LCDMemMap[i * 2 + 12 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 12 + 1] = (value & 0x0F);
	}

	LCDMemMap[14] |= 1;
}

void DisplaySaveMode(void)
{
	memset(LCDMemMap, 0, 16);
	char Array[2] = "";

	if (light.psm == false) {
		// O
		LCDMemMap[10] &= 1;
		LCDMemMap[10] |= 0b1010;
		LCDMemMap[11] |= 0b1111;

		// F
		LCDMemMap[12] &= 1;
		LCDMemMap[12] |= 0b1110;
		LCDMemMap[13] |= 0b0001;

		// F
		LCDMemMap[14] &= 1;
		LCDMemMap[14] |= 0b1110;
		LCDMemMap[15] |= 0b0001;

		return;
	}

	// saveMode on Time
	// sprintf(Array, "%02lu", (unsigned long)onTime->hour);
	sprintf(Array, "%02lu", (unsigned long)light.psmOnTime[0]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 0] &= 1;
		LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 1] = (value & 0x0F);
	}

	// sprintf(Array, "%02lu", (unsigned long)onTime->minute);
	sprintf(Array, "%02lu", (unsigned long)light.psmOnTime[1]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 4 + 0] &= 1;
		LCDMemMap[i * 2 + 4 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 4 + 1] = (value & 0x0F);
	}

	// saveMode off Time
	// sprintf(Array, "%02lu", (unsigned long)offTime->hour);
	sprintf(Array, "%02lu", (unsigned long)light.psmOffTime[0]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 8 + 0] &= 1;
		LCDMemMap[i * 2 + 8 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 8 + 1] = (value & 0x0F);
	}

	// sprintf(Array, "%02lu", (unsigned long)offTime->minute);
	sprintf(Array, "%02lu", (unsigned long)light.psmOffTime[0]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 12 + 0] &= 1;
		LCDMemMap[i * 2 + 12 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 12 + 1] = (value & 0x0F);
	}

	LCDMemMap[6] |= 1;
	LCDMemMap[8] |= 1;
	LCDMemMap[14] |= 1;
}

void DisplayArea(void)
{
	memset(LCDMemMap, 0, 16);
	char Array[2] = "";

	sprintf(Array, "%04lu", (unsigned long)light.region);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 12 + 0] &= 1;
		LCDMemMap[i * 2 + 12 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 12 + 1] = (value & 0x0F);
	}
}

void LCD_SelectMode(uint8_t mode)
{
	LCDMemMap[18] &= 0b1000;
	LCDMemMap[17] = 0;
	LCDMemMap[16] = 0;

	switch (mode) {
	case LCD_CURRENT_DATE:
		LCDMemMap[18] |= 0b0100;
		DisplayDay();
		break;
	case LCD_LIGHTON_TIME:
		LCDMemMap[18] |= 0b0010;
		DisplayTime(mode);
		break;
	case LCD_LIGHTOFF_TIME:
		LCDMemMap[18] |= 0b0001;
		DisplayTime(mode);
		break;
	case LCD_NIGHT_MODE:
		LCDMemMap[17] = 0b0001;
		DisplaySaveMode();
		break;
	case LCD_AREA:
		LCDMemMap[17] = 0b0010;
		DisplayArea();
		break;
	case LCD_CURRENT_TIME:
		LCDMemMap[17] = 0b0100;
		DisplayTime(mode);
		break;
	case LCD_CURRENT_MODE:
		LCDMemMap[17] = 0b1000;
		break;
	default:
		mode = 0;
		LCDMemMap[18] |= 0b0100;
		DisplayDay();
		break;
	}

	switch (light.mode) {
	case LCD_AUTO:
		LCDMemMap[16] = 0b1000;
		break;
	case LCD_SETTING:
		LCDMemMap[16] = 0b0100;
		break;
	default:
		LCDMemMap[16] = 0b1000;
		break;
	}

	currentLCDMode = mode;
	lcd_updateAll();
}

void initLCD(void)
{
	LCD_C_init(LCD_C_BASE, (LCD_C_initParam *)&LCD_C_INIT_PARAM);

	LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_0, LCD_C_SEGMENT_LINE_18);
	LCD_C_clearMemory(LCD_C_BASE);

	// Turn LCD on
	LCD_C_on(LCD_C_BASE);

	LCD_SelectMode(currentLCDMode);
}
