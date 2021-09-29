#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "lcd_c.h"
#include "shell.h"
#include "rtc.h"
#include "metering.h"

#define PSM_NOT_USING 0
#define PSM_RELIGHT 1
#define PSM_NOT_RELIGHT 2

lcdStat_t lcdStat;
char currentLCDMode = LCD_CURRENT_DATE;
extern light_t light;

static uint8_t buttonStatus = BUTTON_NONE;
static uint8_t lcdStatus = LCD_STATUS_NORMAR;

// Local variable
typedef struct {
	char c;
	uint8_t pattern;
} lcd_pattern_t;

const lcd_pattern_t lcdPattern[] = {
	{ '0', 0xAF }, { '1', 0x06 }, { '2', 0xCB }, { '3', 0x4F }, { '4', 0x66 },
	{ '5', 0x6D }, { '6', 0xEC }, { '7', 0x07 }, { '8', 0xEF }, { '9', 0x67 },
	{ '-', 0x40 }, { ' ', 0x00 }, { 'O', 0xAF }, { 'F', 0xE1 }, { 'S', 0x6D },
	{ 'U', 0xAE }, { 'N', 0xA7 }, { 'R', 0xE7 }, { 'I', 0xA0 }, { 'E', 0xE9 },
	{ 'C', 0xA9 }, { 'V', 0xAE }, { 'L', 0xA8 }, { 'T', 0xE8 }, { 'G', 0xAD },
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

		case LCD_PSM_MODE: {
			if (light.psm == PSM_RELIGHT) {
				switch (blinkState) {
				case 1:
					lcd_blink(0, 2);
					light.psmOnTime[0] += offset;
					if (light.psmOnTime[0] > 23) {
						light.psmOnTime[0] -= offset;
					}
					break;

				case 2:
					lcd_blink(2, 4);
					light.psmOnTime[1] += offset;
					if (light.psmOnTime[1] > 59) {
						light.psmOnTime[1] -= offset;
					}
					break;

				case 3:
					lcd_blink(4, 6);
					light.psmOffTime[0] += offset;
					if (light.psmOffTime[0] > 23) {
						light.psmOffTime[0] -= offset;
					}
					break;

				case 4:
					lcd_blink(6, 8);
					light.psmOffTime[1] += offset;
					if (light.psmOffTime[1] > 59) {
						light.psmOffTime[1] -= offset;
					}
					break;

				default:
					lcd_blink(0, 8);
					blinkState = 1;
					light.psm = 0;
					break;
				}
			} else if (light.psm == PSM_NOT_RELIGHT) {
				switch (blinkState) {
				case 1:
					lcd_blink(0, 2);
					light.psmOnTime[0] += offset;
					if (light.psmOnTime[0] > 23) {
						light.psmOnTime[0] -= offset;
					}
					break;

				case 2:
					lcd_blink(2, 4);
					light.psmOnTime[1] += offset;
					if (light.psmOnTime[1] > 59) {
						light.psmOnTime[1] -= offset;
					}
					break;

				default:
					lcd_blink(4, 6);
					blinkState = 3;
					light.psm = PSM_RELIGHT;
					break;
				}
			} else {
				switch (blinkState) {
				case 1:
					lcd_blink(0, 4);
					light.psm = 0;
					break;

				case 2:
					lcd_blink(0, 2);
					blinkState = 1;
					light.psm = PSM_NOT_RELIGHT;
					break;
				}
			}
		} break;

		case LCD_CURRENT_MODE: {
			switch (blinkState) {
			case 1:
				lcd_blink(0, 8);
				if (offset) {
					light.timeBase++;

					if (light.timeBase > 2) {
						light.timeBase = 1;
					}
				}
				break;
			case 2:
				LCD_C_clearBlinkingMemory(LCD_C_BASE);
				LCD_C_setBlinkingMemory(LCD_C_BASE, 16, 0xf);

				if (offset) {
					light.mode++;

					if (light.mode > 2) {
						light.mode = 1;
					}
				}
				break;
			default:
				blinkState = 1;
				lcd_blink(0, 8);
				break;
			}
		} break;

		case LCD_CURRENT_TIME: {
			hour = &rtcData.Hours;
			min = &rtcData.Minutes;
		} break;
		case LCD_LIGHTON_TIME: {
			if (light.mode == LMODE_SETTING) {
				hour = &light.setOnTime[0];
				min = &light.setOnTime[1];
			}
		} break;
		case LCD_LIGHTOFF_TIME: {
			if (light.mode == LMODE_SETTING) {
				hour = &light.setOffTime[0];
				min = &light.setOffTime[1];
			}
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
			}
		}
	} else {
		blinkState = 0;
	}
}

void DisplayDay(void)
{
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

	LCD_DAY_MODE();
}

void DisplayTime(uint8_t mode)
{
	char Array[2] = "";
	Time_t time;

	rtcData = RTC_C_getCalendarTime(RTC_C_BASE);
	fixLcdData();

	switch (mode) {
	case LCD_LIGHTON_TIME:
		if (light.mode == LMODE_SETTING) {
			memcpy(&time, &light.setOnTime, 2);
		} else {
			memcpy(&time, &light.onTime, 2);
		}
		break;
	case LCD_LIGHTOFF_TIME:
		if (light.mode == LMODE_SETTING) {
			memcpy(&time, &light.setOffTime, 2);
		} else {
			memcpy(&time, &light.offTime, 2);
		}
		break;
	case LCD_CURRENT_TIME:
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

	LCD_TIME_MODE();
}

void DisplayPsmMode(void)
{
	fixLcdData();

	LCDMemMap[8] |= 1;

	if (light.psm == false) {
		char *string = "OFF";

		int firstPos = 1;

		for (int i = firstPos; i < 4; i++) {
			uint8_t value = lcd_getCharPattern(*(string + i - firstPos));

			LCDMemMap[i * 2 + 0] &= 1;
			LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
			LCDMemMap[i * 2 + 1] = (value & 0x0F);
		}

		firstPos = 5;
		for (int i = firstPos; i < 8; i++) {
			uint8_t value = lcd_getCharPattern(*(string + i - firstPos));

			LCDMemMap[i * 2 + 0] &= 1;
			LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
			LCDMemMap[i * 2 + 1] = (value & 0x0F);
		}

		return;
	}

	LCDMemMap[6] |= 1;

	if (light.psm == PSM_NOT_RELIGHT) {
		char Array[2] = "";

		// saveMode on Time
		sprintf(Array, "%02lu", (unsigned long)light.psmOnTime[0]);
		for (int i = 0; i < 2; i++) {
			uint8_t value = lcd_getCharPattern(Array[i]);

			LCDMemMap[i * 2 + 0] &= 1;
			LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
			LCDMemMap[i * 2 + 1] = (value & 0x0F);
		}

		sprintf(Array, "%02lu", (unsigned long)light.psmOnTime[1]);
		for (int i = 0; i < 2; i++) {
			uint8_t value = lcd_getCharPattern(Array[i]);

			LCDMemMap[i * 2 + 4 + 0] &= 1;
			LCDMemMap[i * 2 + 4 + 0] |= ((value & 0xE0) >> 4);
			LCDMemMap[i * 2 + 4 + 1] = (value & 0x0F);
		}

		char *string = "OFF";

		int firstPos = 5;
		for (int i = firstPos; i < 8; i++) {
			uint8_t value = lcd_getCharPattern(*(string + i - firstPos));

			LCDMemMap[i * 2 + 0] &= 1;
			LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
			LCDMemMap[i * 2 + 1] = (value & 0x0F);
		}

		return;
	}

	char Array[2] = "";

	// saveMode on Time
	sprintf(Array, "%02lu", (unsigned long)light.psmOnTime[0]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 0] &= 1;
		LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 1] = (value & 0x0F);
	}

	sprintf(Array, "%02lu", (unsigned long)light.psmOnTime[1]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 4 + 0] &= 1;
		LCDMemMap[i * 2 + 4 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 4 + 1] = (value & 0x0F);
	}

	// saveMode off Time
	sprintf(Array, "%02lu", (unsigned long)light.psmOffTime[0]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 8 + 0] &= 1;
		LCDMemMap[i * 2 + 8 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 8 + 1] = (value & 0x0F);
	}

	sprintf(Array, "%02lu", (unsigned long)light.psmOffTime[1]);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 12 + 0] &= 1;
		LCDMemMap[i * 2 + 12 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 12 + 1] = (value & 0x0F);
	}

	LCDMemMap[14] |= 1;
}

void DisplayArea(void)
{
	char Array[2] = "";

	sprintf(Array, "%04lu", (unsigned long)light.region);
	for (int i = 0; i < 2; i++) {
		uint8_t value = lcd_getCharPattern(Array[i]);

		LCDMemMap[i * 2 + 12 + 0] &= 1;
		LCDMemMap[i * 2 + 12 + 0] |= ((value & 0xE0) >> 4);
		LCDMemMap[i * 2 + 12 + 1] = (value & 0x0F);
	}
}

void DisplayCurrentMode(void)
{
	fixLcdData();
#define SUNRISE 1
#define CIVIL 2

	if (light.mode == LMODE_SETTING) {
		char *string = "SETTING";

		for (int i = 0; i < 7; i++) {
			uint8_t value = lcd_getCharPattern(*(string + i));

			LCDMemMap[i * 2 + 0] &= 1;
			LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
			LCDMemMap[i * 2 + 1] = (value & 0x0F);
		}
	} else {
		light.mode = LMODE_INTERNAL;

		if (light.timeBase == SUNRISE) {
			char *string = "SUNRISE";

			for (int i = 0; i < 7; i++) {
				uint8_t value = lcd_getCharPattern(*(string + i));

				LCDMemMap[i * 2 + 0] &= 1;
				LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
				LCDMemMap[i * 2 + 1] = (value & 0x0F);
			}
		} else if (light.timeBase == CIVIL) {
			char *string = "CIVIL";

			for (int i = 0; i < 5; i++) {
				uint8_t value = lcd_getCharPattern(*(string + i));

				LCDMemMap[i * 2 + 0] &= 1;
				LCDMemMap[i * 2 + 0] |= ((value & 0xE0) >> 4);
				LCDMemMap[i * 2 + 1] = (value & 0x0F);
			}
		}
	}
}

void LCD_SelectMode(char mode)
{
	LCD_CLEAR_MODE();
	LCD_CLEAR_DATA();

	switch (mode) {
	case LCD_CURRENT_DATE:
		LCD_TOGGLE_DAY();
		DisplayDay();
		break;
	case LCD_LIGHTON_TIME:
		LCD_TOGGLE_LIGHT_ON();
		DisplayTime(mode);
		break;
	case LCD_LIGHTOFF_TIME:
		LCD_TOGGLE_LIGHT_OFF();
		DisplayTime(mode);
		break;
	case LCD_PSM_MODE:
		LCD_TOGGLE_PSM();
		DisplayPsmMode();
		break;
	case LCD_AREA:
		LCD_TOGGLE_AREA();
		DisplayArea();
		break;
	case LCD_CURRENT_TIME:
		LCD_TOGGLE_CURRENT_TIME();
		DisplayTime(mode);
		break;
	case LCD_CURRENT_MODE:
		LCD_TOGGLE_CURRENT_MODE();
		DisplayCurrentMode();
		break;
	default:
		if (mode < 10) {
			mode = LCD_CURRENT_DATE;
			LCD_TOGGLE_DAY();
			DisplayDay();
		} else {
			mode = LCD_CURRENT_MODE;
			LCD_TOGGLE_CURRENT_MODE();
			DisplayCurrentMode();
		}
		break;
	}

	switch (light.mode) {
	case LCD_AUTO:
		LCD_TOGGLE_AUTO_MODE();
		break;
	case LCD_SETTING:
		LCD_TOGGLE_SET_MODE();
		break;
	default:
		LCD_TOGGLE_AUTO_MODE();
		break;
	}

	currentLCDMode = mode;
	lcd_updateAll();
}

void runLcdCmd(unsigned char lcdCmd)
{
	static light_t prevSet;
	buttonStatus = lcdCmd;

	if (lcdStatus == LCD_STATUS_NORMAR) {
		if (lcdCmd == BUTTON_ENTER) {
			memcpy(&prevSet, &light, sizeof(light_t));
			switch (currentLCDMode) {
			case LCD_AREA:
				break;
			case LCD_LIGHTON_TIME:
			case LCD_LIGHTOFF_TIME:
				if (light.mode == LMODE_INTERNAL) {
					break;
				}
			case LCD_PSM_MODE:
			case LCD_CURRENT_DATE:
			case LCD_CURRENT_TIME:
			case LCD_CURRENT_MODE:
				lcdStatus = LCD_STATUS_BLINK;
				LCD_SelectMode(currentLCDMode);
				break;
			}
		} else if (lcdCmd == BUTTON_STOP) {
			print("req report\n");
			req_report();
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

			if (memcmp(&prevSet, &light, sizeof(light_t))) {
				req_lightFix();
			}
		} else if (lcdCmd == BUTTON_UP) {
			LCD_SelectMode(currentLCDMode);
		} else if (lcdCmd == BUTTON_DOWN) {
			LCD_SelectMode(currentLCDMode);
		}
	}

	buttonStatus = BUTTON_NONE;
}

void LCD_COMM_CTRL(int status)
{
	if (status == COMM_STATUS_ON) {
		LCD_COMM_ON();
	} else if (status == COMM_STATUS_OFF) {
		LCD_COMM_OFF();
	} else if (status == COMM_STATUS_TOGGLE) {
		LCD_COMM_TOGGLE();
	}
	lcd_updateAll();
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
	LCD_C_clearBlinkingMemory(LCD_C_BASE);

	// Turn LCD on
	LCD_C_on(LCD_C_BASE);
	LCD_SelectMode(currentLCDMode);
	/* LCD_COMM_ON(); */
}
