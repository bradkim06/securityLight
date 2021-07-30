#ifndef __LCD_H__
#define __LCD_H__

#include <stdbool.h>
#include <stdint.h>

#define HAL_LCD_MEMMAP_SIZE 19

#define LCD_CURRENT_DATE 0
#define LCD_LIGHTON_TIME 1
#define LCD_LIGHTOFF_TIME 2
#define LCD_NIGHT_MODE 3
#define LCD_AREA 4
#define LCD_CURRENT_TIME 5
#define LCD_CURRENT_MODE 6
#define LCD_AUTO 1
#define LCD_SETTING 2
// #define LCD_DIMMING 3

#define lcd_updateAll() hal_lcd_updatememory(0, HAL_LCD_MEMMAP_SIZE)
#define lcd_update(from, num) hal_lcd_updatememory(from, num)

extern uint8_t currentLCDMode;

void hal_lcd_updatememory(uint8_t baseposition, uint8_t num_segments);
void initLCD(void);
void LCD_clearLCD(void);
void LCD_fullLCD(void);
void LCD_testLCD(char num);
void DisplayDay(void);
void DisplayLightOn(void);
void DisplaySaveMode(void);

void LCD_SelectMode(uint8_t mode);

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
