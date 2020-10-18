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
uint16_t rate;
int main(void)
{
	/*
	 * Instantiation functions. Initializes the GPIOs, enable
	 * interrupts, and plays startup music
	 */
	setupGPIO(); //Initiate GPIO pins for gamepad 

	
	pick_sound(0);
	init_sound();
	*GPIO_PA_DOUT = 0x00;
	uint8_t last_but_reg = 0x00;
	uint8_t but_reg;
	while (1){
		if(playing == false){ //Check for a rising flank of a button signal
			but_reg = *GPIO_PC_DIN; 
			if(but_reg != 0xFF && but_reg!=last_but_reg){
				uint8_t buttonLoc = 0;		
				while (!(((but_reg^0xFF) >> buttonLoc) & 1)) buttonLoc++;
				pick_sound(buttonLoc);  
				*GPIO_PA_DOUT = ~((1<<buttonLoc)<<8);
				init_sound();
			}
			last_but_reg = but_reg;

		}else if(playing == true){
			if(*TIMER1_CNT>=rate){
				push_sound();				
				*TIMER1_CNT -= rate;			
			}
		}
	}
	return 0;
}
void init_sound(){
	playing = true;
	cnt = 0;
	rate = SYSTEM_CLK/playing_sound->sampling_freq;
	enableTimer();
	enableDAC();
}

void push_sound(){
	if (cnt < playing_sound->length) {
		*DAC0_CH0DATA = playing_sound->samples[cnt] << 0;	//Sound values are 8-bit to save memory
		*DAC0_CH1DATA = playing_sound->samples[cnt] << 0;
		cnt++;
	} else if(init_cnt<8){
		pick_sound(init_cnt);
		init_cnt++;
		cnt = 0;
	} else {
		disableTimer();
		disableDAC();
		*GPIO_PA_DOUT = 0xFF << 8;	//turn off LEDs
		cnt = 0;
		playing = false;
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
