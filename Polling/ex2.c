#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "efm32gg.h"
#include "function_def.h"
#include "sounds.h"

#define SYSTEM_CLK 14000000

int i = 0;
// uint8_t init_i = 0;
bool startup = true;
bool playing = false;
uint16_t timer_limit;
int main(void)
{
	char mask = 1;		//Mask bit used in finding button location
	uint8_t last_button = 0x00;
	uint8_t button;

	/*
	 * Instantiation functions. Initializes the GPIOs
	 * and plays startup music
	 */
	
	setupGPIO();		//Initiate GPIO pins for gamepad 

	/*
	   Startup music: Plays first sound number 3,
	   then it will play sound number 2 before 
	   waiting for button press.
	 */

	select_sound(2);
	init_sound();	//Instantiate sound playing
	*GPIO_PA_DOUT = 0x00;
	while (1) {  
		if (playing == true) {
			if (*TIMER1_CNT >= timer_limit) { //Checks if timer count is above set upper limit
				push_sound();				  //New sample is pushed to DAC
				*TIMER1_CNT -= timer_limit;	  //reset timer count
			}
		} else {
			button = *GPIO_PC_DIN ^ 0xFF;	//Invert button values 
			if (button != 0 && button != last_button) {	//Only activate on rising edge
				uint8_t button_loc = 0;

				//Loop through the variable to find the active bit 
				while (!((button >> button_loc) & mask)) //Find pushed button
					button_loc++;
				select_sound(button_loc);				//Select sound according to pushed button
				*GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
				init_sound();
			}
			last_button = button;
		}
	}
	return 0;
}

void init_sound()
{
	/*
  	 * instantiates the playing of a sound
	 */
	playing = true;
	i = 0;
	timer_limit = SYSTEM_CLK / current_sound->sampling_freq;
	enableTimer();
	enableDAC();
}

void push_sound()
{
	/*
	 * Pushes a new sample to the DAC if
	 * there are more samples in the sequence,
	 * otherwise it stops playing, or continues
	 * playing other sounds(i.e for the startup)
	 */
	if (i < current_sound->length) {
		*DAC0_CH0DATA = current_sound->samples[i] << 0;	//Push sample to DAC
		*DAC0_CH1DATA = current_sound->samples[i] << 0;	//Shift samples to amplify sound strength
		i++;
	} else {
		i = 0;
		if (startup) {
			select_sound(1);
			startup = false;
		} else {
			disableTimer();
			disableDAC();
			*GPIO_PA_DOUT = 0xFF << 8;	//turn off LEDs
			playing = false;
		}
	}
}

void setupNVIC()
{
	/*
	 * Interrupt handles are enabled through vector table
	 * We enable interrupt handler for GPIO and timer1
	 */
	*ISER0 |= (ISER0_GPIO_EVEN | ISER0_GPIO_ODD | ISER0_TIMER1);
}
