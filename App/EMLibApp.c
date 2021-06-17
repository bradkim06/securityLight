#include "EMLibApp.h"
#include "EM_userConfig.h"

void initEMLibGUIApp(void)
{
	EM_message_code code;

	/* turn off global interrupts */
	__disable_interrupt();
	// Initialize the Energy Measurement Library
	code = EM_configureEnergyMeasurement(&gEmSWConfig, (EM_SW_Lib_Result_Handle *)&gEmSWResult);

	if (code != EM_message_code_no_error) {
		// Error occurred
		while (1)
			;
	}

	// Initialize run time calibration values
	EM_initalizeCalibrationValues((uint8_t *)g_emPhaseCalibration,
				      (uint8_t *)g_savedEmPhaseCalibration);

	// Initialize DC Filter
	EM_initDCFilters(&gEmSWConfig, (EM_SW_Lib_Result_Handle *)&gEmSWResult);

	// HAL_ADC_init
	HAL_ADC_init(&halAdcConfig);

	// Delay ~200us for 1.2V ref to settle
	__delay_cycles(3200);

	// Start the conversion
	HAL_ADC_startConversion(&halAdcConfig);

	/* turn on global interrupts */
	__enable_interrupt();
}
