#ifndef __LCD_H__
#define __LCD_H__

#include <stdbool.h>
#include <stdint.h>

#define LCD_TOGGLE_DAY() LCDMemMap[18] ^= 0b0100;
#define LCD_TOGGLE_LIGHT_ON() LCDMemMap[18] ^= 0b0010;
#define LCD_TOGGLE_LIGHT_OFF() LCDMemMap[18] ^= 0b0001;
#define LCD_TOGGLE_PSM() LCDMemMap[17] ^= 0b0001;
#define LCD_TOGGLE_AREA() LCDMemMap[17] ^= 0b0010;
#define LCD_TOGGLE_CURRENT_TIME() LCDMemMap[17] ^= 0b0100;
#define LCD_TOGGLE_CURRENT_MODE() LCDMemMap[17] ^= 0b1000;

#define LCD_TOGGLE_AUTO_MODE() LCDMemMap[16] ^= 0b1000;
#define LCD_TOGGLE_SET_MODE() LCDMemMap[16] ^= 0b0100;
#define LCD_TOGGLE_DIMM_MODE() LCDMemMap[16] ^= 0b0010;

#define LCD_DAY_MODE()                                                                             \
	{                                                                                          \
		LCDMemMap[8] |= 1;                                                                 \
		LCDMemMap[12] |= 1;                                                                \
	}
#define LCD_TIME_MODE() LCDMemMap[14] |= 1;
#define LCD_PSM_ON_MODE()                                                                          \
	{                                                                                          \
		LCDMemMap[6] |= 1;                                                                 \
		LCDMemMap[8] |= 1;                                                                 \
		LCDMemMap[14] |= 1;                                                                \
	}

#define LCD_CLEAR_DATA() memset(LCDMemMap, 0, 16);
#define LCD_CLEAR_MODE()                                                                           \
	{                                                                                          \
		LCDMemMap[16] = 0;                                                                 \
		LCDMemMap[17] = 0;                                                                 \
		LCDMemMap[18] = 0;                                                                 \
	}

typedef struct {
	uint8_t dayMode;
} lcdStat_t;

extern char currentLCDMode;
extern lcdStat_t lcdStat;

#define HAL_LCD_MEMMAP_SIZE 19

// LCD Status
#define LCD_STATUS_NORMAR 0
#define LCD_STATUS_BLINK 1

// LCD MODE
#define LCD_CURRENT_DATE 0
#define LCD_LIGHTON_TIME 1
#define LCD_LIGHTOFF_TIME 2
#define LCD_PSM_MODE 3
#define LCD_AREA 4
#define LCD_CURRENT_TIME 5
#define LCD_CURRENT_MODE 6
#define LCD_AUTO 1
#define LCD_SETTING 2
// #define LCD_DIMMING 3

#define lcd_updateAll() hal_lcd_updatememory(0, HAL_LCD_MEMMAP_SIZE)
#define lcd_update(from, num) hal_lcd_updatememory(from, num)
#define lcd_blinkAll() hal_lcd_blinkmemory(0, HAL_LCD_MEMMAP_SIZE)
#define lcd_blink(from, num) hal_lcd_blinkmemory(from, num)

void hal_lcd_updatememory(uint8_t baseposition, uint8_t num_segments);
void hal_lcd_blinkmemory(uint8_t baseposition, uint8_t num_segments);
void initLCD(void);
void LCD_clearLCD(void);
void LCD_fullLCD(void);
void LCD_testLCD(char num);
void DisplayDay(void);
void DisplayLightOn(void);
void DisplayPsmMode(void);

void LCD_SelectMode(char mode);
void runLcdCmd(unsigned char lcdCmd);

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
} currentDate_t;

typedef struct {
	uint8_t hour;
	uint8_t minute;
} Time_t;

typedef struct {
	uint8_t status;
	Time_t onTime;
	Time_t offTime;
} savingMode_t;

typedef struct {
	currentDate_t date;
	Time_t onTime;
	Time_t offTime;
	Time_t currentTime;
	savingMode_t saveMode;
} lcdData_t;
#endif
