// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"


unsigned char Index;  

// 3-bit 16-element sine wave
//const unsigned char SineWave[16] = {8, 11, 13, 14, 15, 14, 13, 11, 8, 5, 3, 2, 1, 2, 3, 5};
const unsigned char SineWave[16] ={0, 1, 2, 5, 8, 10, 13, 14, 15, 14, 13, 10, 8, 5, 2, 1};

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode


// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(unsigned long period){
 	DAC_Init();	
	Index = 0;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
  NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	EnableInterrupts();
  NVIC_ST_RELOAD_R = period-1;// reload value
}

// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	NVIC_ST_RELOAD_R = 0; // clear reload value
	GPIO_PORTB_DATA_R = 0; // clear Port B output
}

// Interrupt service routine
// Executed every 12.5ns*(period)
// RELOAD value is important. controls the rate at which SysTick interrupts
void SysTick_Handler(void){
  Index = (Index+1)&0x0F;  
  DAC_Out(SineWave[Index]); 
}
