// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);

int main(void){ // Real Lab13 
	unsigned long i,input,previous,n;     

	// for the real board grader to work 
	// you must connect PD3 to your DAC output

  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
  // PortE used for piano keys, PortB used for DAC        
	DisableInterrupts();
	Piano_Init();
  Sound_Init(500000); // initialize SysTick timer and DAC
  EnableInterrupts();  // enable after all initialization are done	
	Sound_Off();

	// Initial testing, law of superposition
  DAC_Out(1);
  DAC_Out(2);
  DAC_Out(4);
	
// static testing, single step and record Vout
  for(i=0;i<8;i++){
    DAC_Out(i);
  }
	
	previous = Piano_In();
	
//	n = 0;
//  TExaS_Init(SW_PIN_PA5432, DAC_PIN_PB3210,ScopeOn); // activate grader 
//	
//	// static test
//	while(1){
//    DAC_Out(n);     // your code to output to the DAC
//    delay(1); // wait 1s (later change this to 1ms)
//    n = (n+1)&0x0F;
//  }
	
  while(1) {              
		// input from keys to select tone
		input = Piano_In(); // means a switch is pressed
		if (input) {
			// Piano key 0
			if(input == 0x01) {
				Sound_Tone(C0);
			}
			// Piano key 1
			else if(input == 0x02) {
				Sound_Tone(D);
			}
			// Piano key 2
			else if(input == 0x04) {
				Sound_Tone(E);
			}
			// Piano key 3
			else if(input == 0x08) {
				Sound_Tone(G);
			}
		} 

		else {
			Sound_Off();
		}	
		
		if(previous&&(input==0)){ // just released     
			Sound_Off();
		}		
		
		delay(10);
		previous = input;		
  }            
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
                count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


