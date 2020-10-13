/*
* Declarations of the functions used in the different
* c files. 
*/

void enableTimer(uint16_t period);
void enableDAC();
void setupNVIC();
void setupGPIO();
void pick_sound(uint8_t button);
void disableTimer();
void button_handler(uint8_t init);
void disableDAC();
