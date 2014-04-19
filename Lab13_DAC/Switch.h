// Switch.h
// This software configures the on-board switches and LEDs.
//
// For use with the LM4F120 or TM4C123
// Port E is onboard LaunchPad switches and LED
// Port E bit 4 is positive logic switch to play sound, SW1-4
// TODO: SysTick ISR: PE3 ISR heartbeat??


//---------------------Switch_Init---------------------
// initialize switch interface
// Input: none
// Output: none 
void Switch_Init(void);

//---------------------Switch_In---------------------
// read the values of the two switches
// Input: none
// Output: 0x00,0x01,0x10,0x11 from the two switches
//         0 if no switch is pressed
// bit3 PE3 SW4 switch
// bit2 PE2 SW3 switch
// bit1 PE1 SW2 switch
// bit0 PE0 SW1 switch


unsigned long Switch_In(void); 

//---------------------Delay10ms---------------------
// wait 10ms for switches to stop bouncing
// Input: none
// Output: none
void Delay10ms(void);
