// Switch.c
// This software configures the on-board switches and LEDs.
//
// For use with the LM4F120 or TM4C123
// Port E is onboard LaunchPad switches and LED
// Port E bit 4 is negative logic switch to play sound, SW1
// SysTick ISR: PF3 ISR heartbeat

#include "../tm4c123gh6pm.h"

//---------------------Switch_Init---------------------
// initialize switch interface
// Input: none
// Output: none 
void Switch_Init(void){ volatile unsigned long  delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port E
  GPIO_PORTE_CR_R = 0x1F;           // allow changes to PE3-0
  // only PE0 needs to be unlocked, other bits can't be locked
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PE
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE3-0
  GPIO_PORTE_DIR_R = 0x0F;          // 5) PE3-0 out
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PE7-0
  GPIO_PORTE_PUR_R = 0x0F;          // enable pull-up on PE0-3
  GPIO_PORTE_DEN_R = 0x0F;          // 7) enable digital I/O on PE3-0
}

//---------------------Switch_In---------------------
// read the values of the two switches
// Input: none
// Output: 0x00,0x01,0x10,0x11 from the two switches
//         0 if no switch is pressed
// bit4 PF4 SW1 switch
// bit0 PF0 SW2 switch 
unsigned long Switch_In(void){ 
  return (GPIO_PORTF_DATA_R&0x11)^0x11;
}
//---------------------Delay10ms---------------------
// wait 10ms for switches to stop bouncing
// Input: none
// Output: none
void Delay10ms(void){unsigned long volatile time;
  time = 14545;  // 10msec
  while(time){
		time--;
  }
}

