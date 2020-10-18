#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "efm32gg.h"
#include "function_def.h"
#include "sounds.h"

#define SYSTEM_CLK 14000000


int cnt = 0;
uint8_t init_cnt = 0;
bool playing = false;

int main(void)
{
	/*
	 * Instantiation functions. Initializes the GPIOs, enable
	 * interrupts, and plays startup music
	 */
	setupGPIO(); //Initiate GPIO pins for gamepad 

	init_sound();
	pick_sound(0);
	*GPIO_PA_DOUT = 0x00;
	uint8_t last_but_reg = 0x00;
	uint8_t but_reg;
	while (1){
		but_reg = *GPIO_PC_DIN; 
		if(playing == false && but_reg != 0xFF && but_reg!=last_but_reg){ //Check for a rising flank of a button signal
			uint8_t id = 0;		
			for(; id < 8; id++){ // Figure out which button is pressed. Id = 0 - 7
				if(((~but_reg>>id) & 0x01) == 1){
					pick_sound(id);  
					break;
				}
			}
			*GPIO_PA_DOUT = ~((1<<id)<<8);
			init_sound();
		}else if(playing == true){
			push_sound();
		}
		last_but_reg = but_reg;
	}
	return 0;
}
void init_sound(){
	playing = true;
	cnt = 0;
	enableTimer();
	enableDAC();
}
void push_sound(){
	while(1){
		if(*TIMER1_CNT>=SYSTEM_CLK/playing_sound->sampling_freq){
			cnt++;
			*TIMER1_CNT = 0;
		}
		if (cnt < playing_sound->length) {
			*DAC0_CH0DATA = playing_sound->samples[cnt] << 0;	//Sound values are 8-bit to save memory
			*DAC0_CH1DATA = playing_sound->samples[cnt] << 0;
		} else if(init_cnt<8){
			pick_sound(init_cnt);
			init_cnt++;
			cnt = 0;
		} else if(cnt >= playing_sound->length) {
			disableTimer();
			disableDAC();
			*GPIO_PA_DOUT = 0xFF << 8;	//turn off LEDs
			cnt = 0;
			playing = false;
		}
	}
}


void setupNVIC()
{
	/*
	 * Interrupt handles are enabled through vector table
	 */
	*ISER0 |= (ISER0_GPIO_EVEN | ISER0_GPIO_ODD | ISER0_TIMER1);
}