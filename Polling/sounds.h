#include <stdint.h>

/*
 * Declaration of a struct that will hold
 * all sound effect data: length of sequence,
 * sampling_frequency, and sequence of samples
 * Current sound pointer is declared here
 */

typedef struct Sound {
  uint32_t length;
  uint16_t sampling_freq;
  uint8_t samples[];
}Sound;

Sound *playing_sound;
