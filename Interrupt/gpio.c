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

	/* enable high frequency GPIO clock */
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;

	/* Setup sequence for outputs: set high drive strength,
	 * set pins A8-15 as output, and turn all LEDs off */
	*GPIO_PA_CTRL = 0x02;
	*GPIO_PA_MODEH = 0x55555555;
	*GPIO_PA_DOUT = 0xFF << 8;

	/* Setup sequence for inputs: Configure buttons as inputs with pull
	 * up resistor and a filter*/
	*GPIO_PC_MODEL = 0x33333333;
	*GPIO_PC_DOUT = 0xFF;

	/* Setup sequence for interrupt: Select external interrupt from pins
	 * 0-7 of PC, Enable interrupts for pins 0-7 and enable interrupt for
	 * rising and falling edge */
	*GPIO_EXTIPSELL = 0x22222222;
	*GPIO_IEN = 0xFF;
	*GPIO_EXTIRISE = 0xFF;
	*GPIO_EXTIFALL = 0xFF;
}
