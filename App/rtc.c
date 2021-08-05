#include "rtc.h"
#include "shell.h"
#include "flash.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

Calendar rtcData = { 0 };

static uint16_t dayofweek(uint16_t d, uint16_t m, uint16_t y)
{
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	y -= m < 3;
	return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

static void cvt_TIME(char const *date, char const *time, Calendar *rtc)
{
	char s_month[5];
	uint16_t month, day, year;
	static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	sscanf(date, "%s %u %u", s_month, &day, &year);

	month = (strstr(month_names, s_month) - month_names) / 3;
	uint16_t weekday = dayofweek(day, month + 1, year);

	rtc->Month = month;
	rtc->DayOfMonth = day;
	rtc->DayOfWeek = weekday;
	rtc->Year = year;

	uint16_t hour, minute, second;

	sscanf(time, "%2u %*c %2u %*c %2u", &hour, &minute, &second);

	rtc->Hours = hour;
	rtc->Minutes = minute;
	rtc->Seconds = second;
}

void initRTC(void)
{
	//Initialize calendar struct
	Calendar currentTime;
	cvt_TIME(__DATE__, __TIME__, &currentTime);

	//Initialize alarm struct
	// RTC_C_configureCalendarAlarmParam alarmParam;
	// alarmParam.minutesAlarm = 27;
	// alarmParam.hoursAlarm = currentTime.Hours;
	// alarmParam.dayOfWeekAlarm = currentTime.DayOfWeek;
	// alarmParam.dayOfMonthAlarm = currentTime.DayOfMonth;
	//Initialize Calendar Mode of RTC C
	/*
    * Base Address of the RTC C A
    * Pass in current time, initialized above
    * Use BCD as Calendar Register Format
    */
	RTC_C_initCalendar(RTC_C_BASE, &currentTime, RTC_C_FORMAT_BINARY);
	// RTC_C_configureCalendarAlarm(RTC_C_BASE, &alarmParam);
	//Specify an interrupt to assert every noon
	RTC_C_setCalendarEvent(RTC_C_BASE, RTC_C_CALENDAREVENT_NOON);
	//Enable interrupt for RTC C Ready Status, which asserts when the RTC C
	//Calendar registers are ready to read.
	//Also, enable interrupts for the Calendar alarm and Calendar event.
	// RTC_C_enableInterrupt(RTC_C_BASE, RTC_C_CLOCK_READ_READY_INTERRUPT +
	// 					  RTC_C_TIME_EVENT_INTERRUPT +
	// 					  RTC_C_CLOCK_ALARM_INTERRUPT);
	RTC_C_enableInterrupt(RTC_C_BASE, RTC_C_TIME_EVENT_INTERRUPT);
	//Start RTC C Clock
	RTC_C_startClock(RTC_C_BASE);
}

// RTC Interrupt Service Routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = RTC_VECTOR
__interrupt void rtc_isr(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(RTC_VECTOR))) rtc_isr(void)
#else
#error Compiler not supported!
#endif
{
	switch (__even_in_range(RTCIV, 16)) {
	case RTCIV_NONE: // No interrupts
		break;
	case RTCIV_RTCOFIFG: // RTCOFIFG
		break;
	case RTCIV_RTCRDYIFG: // RTCRDYIFG second event
		print("sec event!\n");
		break;
	case RTCIV_RTCTEVIFG: // RTCEVIFG hour event
		print("noon event, flash write\n");
		writeFlash();
		break;
	case RTCIV_RTCAIFG: // RTCAIFG alarm event
		print("alarm event!\n");
		break;
	case RTCIV_RT0PSIFG: // RT0PSIFG
		break;
	case RTCIV_RT1PSIFG: // RT1PSIFG
		break;
	case 14:
		break; // Reserved
	case 16:
		break; // Reserved
	default:
		break;
	}
}
