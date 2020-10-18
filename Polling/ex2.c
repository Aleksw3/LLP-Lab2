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
  setupGPIO();    // Initiate GPIO pins for gamepad
  init_sound();   // Initialize variables and perifery for sound playback
  pick_sound(0);  // Sound to start with
  *GPIO_PA_DOUT = 0x00 // Turn LEDs on
  uint8_t last_but_reg = 0x00;
  uint8_t but_reg;
  while (1){
    but_reg = *GPIO_PC_DIN;  // Read the values from buttons
    // Check for a rising flank of a button signal, and not playing sound
    if(playing == false && but_reg != 0xFF && but_reg != last_but_reg){
      uint8_t id = 0;
      for(; id < 8; id++){	 // Test if each button is pressed. Id = 0 - 7
        if(((~but_reg >> id) & 0x01) == 1){
          pick_sound(id);    // Pick sound according to button pressed
          break;
        }
      }
      *GPIO_PA_DOUT = ~((1 << id) << 8); // LED shows button pressed
      init_sound();  // Initialize variables and periphery for sound playback
    }else if(playing == true){
			// Play next soundsample at the correct time
      if(*TIMER1_CNT >= SYSTEM_CLK / (playing_sound -> sampling_freq)){
        push_sound();
        *TIMER1_CNT = 0;
      }
    }
    last_but_reg = but_reg; // Update for checking rising flank
  }
  return 0;
}

void init_sound()
{
  /*
  * To be able to play sounds, we initialize related variables and call
  * functions to enable Timer and DAC.
  */
  playing = true;  // true => play all sounds at initializing
  cnt = 0;
  enableTimer();
  enableDAC();
}

void push_sound(){
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
}


void setupNVIC()
{
  /*
   * Interrupt handles are enabled through vector table
	 * We enable interrupt handler for GPIO and timer1
   */
  *ISER0 |= (ISER0_GPIO_EVEN | ISER0_GPIO_ODD | ISER0_TIMER1);
}
