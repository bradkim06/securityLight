#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "lcd_c.h"
#include "shell.h"
#include "rtc.h"
#include "metering.h"

#define BUTTON_NONE 0
#define BUTTON_ENTER 4
#define BUTTON_STOP 5
#define BUTTON_UP 6
#define BUTTON_DOWN 7

lcdStat_t lcdStat;
char currentLCDMode = LCD_CURRENT_DATE;
extern light_t light;

static bool ChgLightConfig = 0;
static uint8_t buttonStatus = BUTTON_NONE;
static uint8_t lcdStatus = LCD_STATUS_NORMAR;

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

void hal_lcd_blinkmemory(uint8_t baseposition, uint8_t num_segments)
{
	LCD_C_clearBlinkingMemory(LCD_C_BASE);

	uint8_t i;
	uint8_t blinkMemMap[HAL_LCD_MEMMAP_SIZE] = { 0 };

	for (i = baseposition; i < num_segments; i++) {
		blinkMemMap[i * 2 + 0] &= 1;
		blinkMemMap[i * 2 + 0] |= 0xE0 >> 4;
		blinkMemMap[i * 2 + 1] = 0x0F;
	}

	for (i = baseposition; i < num_segments * 2; i++) {
		LCD_C_setBlinkingMemory(LCD_C_BASE, baseposition + i,
					blinkMemMap[baseposition + i]);
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

static void fixLcdData()
{
#define YEAR 1
#define MONTH 2
#define DAY 3

#define HOUR 1
#define MINUTE 2
	static char blinkState = 0;

	if (lcdStatus == LCD_STATUS_BLINK) {
		signed char offset = 0;
		uint8_t *hour, *min;

		if (buttonStatus == BUTTON_ENTER) {
			blinkState++;
		}

		if (buttonStatus == BUTTON_UP) {
			offset++;
		} else if (buttonStatus == BUTTON_DOWN) {
			offset--;
		}

		switch (currentLCDMode) {
		case LCD_CURRENT_DATE: {
			switch (blinkState) {
			case YEAR:
				lcd_blink(0, 4);
				rtcData.Year += offset;
				break;
			case MONTH:
				lcd_blink(4, 6);
				rtcData.Month += offset;
				if (rtcData.Month < 1 || rtcData.Month > 12) {
					rtcData.Month -= offset;
				}
				break;
			case DAY:
				lcd_blink(6, 8);
				rtcData.DayOfMonth += offset;
				if (rtcData.DayOfMonth < 1 || rtcData.DayOfMonth > 31) {
					rtcData.DayOfMonth -= offset;
				}
				break;
			default:
				blinkState = YEAR;
				lcd_blink(0, 4);
				break;
			}

		} break;

		case LCD_CURRENT_TIME: {
			hour = &rtcData.Hours;
			min = &rtcData.Minutes;
		} break;
		case LCD_LIGHTON_TIME: {
			hour = &light.onTime[0];
			min = &light.onTime[1];
		} break;
		case LCD_LIGHTOFF_TIME: {
			hour = &light.offTime[0];
			min = &light.offTime[1];
		} break;
		default:
			break;
		}

		switch (currentLCDMode) {
		case LCD_CURRENT_TIME:
		case LCD_LIGHTON_TIME:
		case LCD_LIGHTOFF_TIME:
			switch (blinkState) {
			case HOUR: {
				lcd_blink(4, 6);
				*hour += offset;
				if (*hour > 23) {
					*hour -= offset;
				}
			} break;
			case MINUTE: {
				lcd_blink(6, 8);
				*min += offset;
				if (*min > 59) {
					*min -= offset;
				}
			} break;
			default: {
				blinkState = HOUR;
				lcd_blink(4, 6);
			} break;
			}
			break;
		default:
			break;
		}

		if (offset) {
			switch (currentLCDMode) {
			case LCD_CURRENT_DATE:
			case LCD_CURRENT_TIME:
				RTC_C_initCalendar(RTC_C_BASE, &rtcData, RTC_C_FORMAT_BINARY);
				RTC_C_setCalendarEvent(RTC_C_BASE, RTC_C_CALENDAREVENT_NOON);
				RTC_C_enableInterrupt(RTC_C_BASE, RTC_C_TIME_EVENT_INTERRUPT);
				RTC_C_startClock(RTC_C_BASE);
				break;

			case LCD_LIGHTON_TIME:
			case LCD_LIGHTOFF_TIME:
				ChgLightConfig = 1;
				break;
			}
		}
	} else {
		blinkState = 0;
	}
}

void DisplayDay(void)
{
	memset(LCDMemMap, 0, 16);
	char Array[4] = "";

	rtcData = RTC_C_getCalendarTime(RTC_C_BASE);
	fixLcdData();

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
		fixLcdData();
		memcpy(&time, &light.onTime, 2);
		break;
	case LCD_LIGHTOFF_TIME:
		fixLcdData();
		memcpy(&time, &light.offTime, 2);
		break;
	case LCD_CURRENT_TIME:
		rtcData = RTC_C_getCalendarTime(RTC_C_BASE);
		fixLcdData();
		time.hour = rtcData.Hours;
		time.minute = rtcData.Minutes;
		break;
	case LCD_CURRENT_MODE:
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

void LCD_SelectMode(char mode)
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
		if (mode < 10) {
			mode = LCD_CURRENT_DATE;
			LCDMemMap[18] |= 0b0100;
			DisplayDay();
		} else {
			mode = LCD_CURRENT_MODE;
			LCDMemMap[17] = 0b1000;
			DisplayTime(mode);
		}
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

void runBlinkCmd(int button)
{
}

void runLcdCmd(unsigned char lcdCmd)
{
	buttonStatus = lcdCmd;

	if (lcdStatus == LCD_STATUS_NORMAR) {
		if (lcdCmd == BUTTON_ENTER) {
			switch (currentLCDMode) {
			case LCD_CURRENT_MODE:
				break;
			case LCD_NIGHT_MODE:
				break;
			case LCD_AREA:
				break;
			case LCD_CURRENT_DATE:
			case LCD_LIGHTON_TIME:
			case LCD_LIGHTOFF_TIME:
			case LCD_CURRENT_TIME:
				lcdStatus = LCD_STATUS_BLINK;
				LCD_SelectMode(currentLCDMode);
				break;
			}
		} else if (lcdCmd == BUTTON_STOP) {
		} else if (lcdCmd == BUTTON_UP) {
			LCD_SelectMode(++currentLCDMode);
		} else if (lcdCmd == BUTTON_DOWN) {
			LCD_SelectMode(--currentLCDMode);
		}
	} else if (lcdStatus == LCD_STATUS_BLINK) {
		if (lcdCmd == BUTTON_ENTER) {
			LCD_SelectMode(currentLCDMode);
		} else if (lcdCmd == BUTTON_STOP) {
			lcdStatus = LCD_STATUS_NORMAR;
			LCD_C_clearBlinkingMemory(LCD_C_BASE);
			LCD_SelectMode(currentLCDMode);
			if (ChgLightConfig) {
				req_lightFix();
				ChgLightConfig = 0;
			}
		} else if (lcdCmd == BUTTON_UP) {
			LCD_SelectMode(currentLCDMode);
		} else if (lcdCmd == BUTTON_DOWN) {
			LCD_SelectMode(currentLCDMode);
		}
	}

	buttonStatus = BUTTON_NONE;
}

void initLCD(void)
{
	lcdStat.dayMode = LCD_AUTO;

	LCD_C_init(LCD_C_BASE, (LCD_C_initParam *)&LCD_C_INIT_PARAM);

	LCD_C_setPinAsLCDFunctionEx(LCD_C_BASE, LCD_C_SEGMENT_LINE_0, LCD_C_SEGMENT_LINE_18);
	LCD_C_clearMemory(LCD_C_BASE);
	LCD_C_setBlinkingControl(LCD_C_BASE, LCD_C_BLINK_FREQ_CLOCK_DIVIDER_6,
				 LCD_C_BLINK_FREQ_CLOCK_PRESCALAR_1024,
				 LCD_C_BLINK_MODE_INDIVIDUAL_SEGMENTS);

	// Turn LCD on
	LCD_C_on(LCD_C_BASE);

	LCD_SelectMode(currentLCDMode);
}
