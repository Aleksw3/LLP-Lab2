#include <stdint.h>
#include <stdbool.h>		//This allows us to use booleans
#include <math.h>

#include "efm32gg.h"
#include "sounds.h"
#include "function_def.h"

#define   SYSTEM_CLK	  14000000

int cnt = 0;
bool playing = false;
uint8_t init_cnt = 0;

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	 * Send new sound sample to the DAC from currently playing sound.
	 * Stops the timer and DAC if all the samples of the sound effect
	 * has been played
	 */
	 if (playing == true) {
	   if (cnt < playing_sound->length) {   // Sound playback if not end of sound
	     // Sound values are 8-bit. Shift left to increase volume
	     *DAC0_CH0DATA = playing_sound->samples[cnt] << 0;
	     *DAC0_CH1DATA = playing_sound->samples[cnt] << 0;
	     cnt++;
	   } else if (init_cnt < 8) {   // Play next part of startup sound
	     pick_sound(init_cnt);
	     init_cnt++;
	     cnt = 0;
	   } else {	// Disable periphery related to sound playback
	     disableTimer();
	     disableDAC();
	     *GPIO_PA_DOUT = 0xFF << 8;  // Turn off LEDs
	     *SCR = 0x06;  // Allows deepsleep
	     cnt = 0;
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
	if (init == 1) { // Initial sound effects
		init_cnt++;
		playing = true;
		pick_sound(1);
		enableTimer(SYSTEM_CLK / playing_sound->sampling_freq);
		enableDAC();
	} else {
		uint32_t but_reg = *GPIO_PC_DIN; // Read the values from buttons
		if (playing == false && but_reg != 0xFF) {
			playing = true;
			uint8_t id = 0;
			for (; id < 8; id++) {	// Figure out which button is pressed. Id = 0 - 7
				if (((~but_reg >> id) & 0x01) == 1) {
					pick_sound(id); // Pick sound according to button pressed
					break;
				}
			}
			*GPIO_PA_DOUT = ~((1 << id) << 8);	// Enable LED according to sound playing
			*SCR = 0x04; // Disables return to sleep when exiting interrupt handler
			//Initialize timer according to sequence sampling frequency
			enableTimer(SYSTEM_CLK / playing_sound->sampling_freq);
			enableDAC();
		}
	}
}
