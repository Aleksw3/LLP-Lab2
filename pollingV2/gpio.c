#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "function_def.h"


void setupGPIO()
{
	/*
	 * Configures GPIOs for LEDs and buttons.
	 * Button GPIOs are configured to use interrupts
	 */

	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	/* enable GPIO clock */
	*GPIO_PA_CTRL = 0x02;			/* set high drive strength */
	*GPIO_PA_MODEH = 0x55555555;	/* set pins A8-15 as output */
	*GPIO_PA_DOUT = 0xFF << 8;		/* off all leds */

	*GPIO_PC_MODEL = 0x33333333;	//Configure button GPIO as inputs with pull(up or down depending on dout) and a filter
	*GPIO_PC_DOUT = 0xFF;			// Configure the input GPIOs to use pull-ups

	*GPIO_EXTIPSELL = 0x22222222;	// Enable pins 0-7 of PC as interrupts
	*GPIO_IEN = 0xFF;				//Enable interrupts for pins 0-7 of the buttons
	*GPIO_EXTIRISE = 0xFF;			//Only enable GPIO interrupt for rising edge
	*GPIO_EXTIFALL = 0xFF;
}
