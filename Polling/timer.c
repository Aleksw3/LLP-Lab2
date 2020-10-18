#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "function_def.h"
#include "sounds.h"

/*
 * function to setup the timer
 */
void enableTimer()
{
	/*
	 * Enable timer peripheral by connecting HF-clock,
	 * set top value/limit before overflow of counter
	 * to period, enable interrupt on overflow, and
	 * start counter
	 */
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;	// Enable clock
	*TIMER1_CMD = 0x01;	//Start counting
}

void disableTimer()
{
	/*
	 * Stop the timer from counting and reset count value
	 */
	*TIMER1_CMD = 0x02;
	*TIMER1_CNT = 0;
}
