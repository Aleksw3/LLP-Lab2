#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "function_def.h"

void enableDAC()
{
	/*
	 * Configures the DAC to be ready to output an analog signal
	 * to a pin
	 */
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_DAC0;
	*DAC0_CTRL = 0x50010;	// Enable DAC output to amplifier, frequency = 437.6kHz
	*DAC0_CH0CTRL = 0x01; // Enable channel 0
	*DAC0_CH1CTRL = 0x01; // Enable channel 1
}


void disableDAC() 
{
	/*
	 * Completely disables the DAC
	 */
	*DAC0_CH0CTRL = 0x00;	// Disable channel 0
	*DAC0_CH1CTRL = 0x00;	// Disable channel 1
	*DAC0_CTRL = 0x00;    // Disable output to pin
}
