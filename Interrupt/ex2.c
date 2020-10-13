#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "efm32gg.h"
#include "function_def.h"

int main(void)
{
	/*
	 * Instantiation functions. Initializes the GPIOs, enable
	 * interrupts, and plays startup music
	 */
	setupGPIO(); //Initiate GPIO pins for gamepad
	setupNVIC(); //Enable interrupt handling 
	button_handler(1); // Play startup music
	return 0;
}

void setupNVIC()
{
	/*
	 * Interrupt handles are enabled through vector table
	 */
	*ISER0 |= (ISER0_GPIO_EVEN | ISER0_GPIO_ODD | ISER0_TIMER1);
}