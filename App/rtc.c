#include "rtc.h"
#include "shell.h"

Calendar rtcData = {
	.Year = 2011,
	.Month = 12,
	.DayOfMonth = 05,
	.DayOfWeek = 03,
	.Hours = 24,
	.Minutes = 59,
	.Seconds = 55,
};

void initRTC(void)
{
	// Initialize alarm struct
	// RTC_C_configureCalendarAlarmParam alarmParam;
	// alarmParam.minutesAlarm    = 0x00;
	// alarmParam.hoursAlarm      = 0x17;
	// alarmParam.dayOfWeekAlarm  = RTC_C_ALARMCONDITION_OFF;
	// alarmParam.dayOfMonthAlarm = 0x05;

	// Initialize Calendar Mode of RTC C
	// RTC_C_initCalendar(RTC_C_BASE, &rtcData, RTC_C_FORMAT_BINARY);
	// RTC_C_configureCalendarAlarm(RTC_C_BASE, &alarmParam);
	// RTC_C_setCalendarEvent(RTC_C_BASE, RTC_C_CALENDAREVENT_MINUTECHANGE);
	// RTC_C_enableInterrupt(RTC_C_BASE, RTC_C_CLOCK_READ_READY_INTERRUPT +
	//                                       RTC_C_TIME_EVENT_INTERRUPT +
	//                                       RTC_C_CLOCK_ALARM_INTERRUPT);
	// RTC_C_startClock(RTC_C_BASE);

	// Loop until XT1, XT2 & DCO fault flag is cleared
	do {
		UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);
		// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG; // Clear fault flags
	} while (SFRIFG1 & OFIFG); // Test oscillator fault flag

	// Configure RTC_C
	RTCCTL0_H = RTCKEY_H; // Unlock RTC_C module
	RTCCTL0_L |=
		RTCTEVIE | RTCAIE | RTCRDYIE; // Enable RTC time event, alarm
	// event, read ready interrupt
	RTCCTL1 |= RTCBCD | RTCHOLD; // RTC enable BCD mode, RTC hold

	RTCYEAR = 0x2011; // Year = 0x2011 = 2011
	RTCMON = 0x12; // Month = 0x12 = December
	RTCDAY = 0x05; // Day = 0x05 = 5th
	RTCDOW = 0x03; // Day of week = 0x03 = Wednesday
	RTCHOUR = 0x24; // Hour = 0x12
	RTCMIN = 0x59; // Minute = 0x57
	RTCSEC = 0x55; // Seconds = 0x36

	RTCADOWDAY = 0x3; // RTC Day of week alarm = 0x2
	RTCADAY = 0x22; // RTC Day Alarm = 0x22
	RTCAHOUR = 0x23; // RTC Hour Alarm
	RTCAMIN = 0x45; // RTC Minute Alarm

	RTCCTL1 &= ~(RTCHOLD); // Start RTC calendar mode
	RTCCTL0_H = 0; // Lock RTC_C module
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
	case RTCIV_RTCRDYIFG: // RTCRDYIFG
		__no_operation(); // Interrupts every minute
		print("second\n");
		break;
	case RTCIV_RTCTEVIFG: // RTCEVIFG
		__no_operation(); // Interrupts every minute
		print("minute\n");
		break;
	case RTCIV_RTCAIFG: // RTCAIFG
		__no_operation(); // Interrupts every alarm event
		print("alarm\n");
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
