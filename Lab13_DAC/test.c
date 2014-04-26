// Static testing The first step in debugging the hardware is to build the DAC, and write DAC_Init and DAC_Out. For this test you do not connect the headphones. If you connect PD3 to the DAC output and run this program with 1 second delay you should see the voltage on the voltmeter. You can add n to the watch window to compare the DAC digital input to the DAC analog output.


#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"

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

unsigned long n;
int main(void){
  n = 0;
  TExaS_Init(SW_PIN_PA5432, DAC_PIN_PB3210,SCOPE); // activate grader 
  DAC_Init();   // your code to initialize the DAC
  EnableInterrupts();
  while(1){
    DAC_Out(n);     // your code to output to the DAC
    delay(1000); // wait 1s (later change this to 1ms)
    n = (n+1)&0x0F;
  }
}