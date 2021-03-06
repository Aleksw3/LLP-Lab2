#include <stdint.h>
#include <stdbool.h>		//This allows us to use booleans

#include "efm32gg.h"
#include "sounds.h"
#include "function_def.h"

#define   SYSTEM_CLK	  14000000

int i = 0;
bool playing = false;
uint8_t init_i = 0;

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	 * Send new sound sample to the DAC from currently playing sound.
	 * Stops the timer and DAC if all the samples of the sound effect
	 * has been played
	 */
	if (playing == true) {
		if (i < current_sound->length) {	// Sound playback if not end of sound
			// Sound values are 8-bit. Shift left to increase volume
			*DAC0_CH0DATA = current_sound->samples[i] << 0;
			*DAC0_CH1DATA = current_sound->samples[i] << 0;
			i++;
		} else if (init_i < 8) {	// Play next part of startup sound
			select_sound(init_i);
			init_i++;
			i = 0;
		} else {	// Disable periphery related to sound playback
			disableTimer();
			disableDAC();
			*GPIO_PA_DOUT = 0xFF << 8;	// Turn off LEDs
			*SCR = 0x06;	// Allows deepsleep
			i = 0;
			playing = false;
		}
	}
	*TIMER1_IFC = *TIMER1_IF;	//Clear interrupt flags
}

void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	/*
	 * GPIO even pin interrupt handler
	 */
	button_handler(0);
	*GPIO_IFC = *GPIO_IF;	//Clear interrupt flags
}

void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/*
	 * GPIO odd pin interrupt handler
	 */
	button_handler(0);
	*GPIO_IFC = *GPIO_IF;	//Clear interrupt flags
}

void button_handler(uint8_t init)
{
	/*
	 * Enable and select sound to be played
	 * Enable DAC and Timer
	 * Enable LED corresponding to sound being played
	 * Disable sleep mode after leaving interrupt handle
	 */
	if (init == 1) {	// Initial sound effects
		init_i++;
		playing = true;
		*GPIO_PA_DOUT = 0x00;
		select_sound(1);
		enableTimer(SYSTEM_CLK / current_sound->sampling_freq);
		enableDAC();
	} else {
		uint32_t button = *GPIO_PC_DIN ^ 0xFF;	// Read the values from buttons
		uint8_t mask = 1;
		if (playing == false && button != 0) {
			playing = true;
			uint8_t button_loc = 0;
			//Loop through the variable to find the active bit 
			while (!((button >> button_loc) & mask))
				button_loc++;
			select_sound(button_loc);
			*GPIO_PA_DOUT = *GPIO_PA_DIN << 8;	// Enable LED according to sound playing
			*SCR = 0x04;	// Disables return to sleep when exiting interrupt handler
			//Initialize timer according to sequence sampling frequency
			enableTimer(SYSTEM_CLK / current_sound->sampling_freq);
			enableDAC();
		}
	}
}
