// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"

// ***** 2. Global Declarations Section *****
const unsigned long SIM_FACTOR = 1;
const unsigned long GREEN_DELAY = 30 * SIM_FACTOR;
const unsigned long YELLOW_DELAY = 5 * SIM_FACTOR;
const unsigned long WALK_DELAY = 10 * SIM_FACTOR;
const unsigned long DONT_WALK_DELAY = 1 * SIM_FACTOR;
const unsigned long HURRY_DELAY = 1 * SIM_FACTOR;
const unsigned long ALLSTOP_DELAY = 5 * SIM_FACTOR;

const unsigned long RED = 0X02;
const unsigned long GREEN = 0x08;


// register addresses
#define TRAFFICLIGHT 	          (*((volatile unsigned long *)0x400050FC))
#define GPIO_PORTB_OUT          (*((volatile unsigned long *)0x400050FC)) // bits 5-0
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTE_IN           (*((volatile unsigned long *)0x4002401C)) // bits 2-0
#define SENSOR                  (*((volatile unsigned long *)0x4002401C))

#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // port B Clock Gating Control

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define PF4                     (*((volatile unsigned long *)0x40025040))
#define PF3                     (*((volatile unsigned long *)0x40025020))
#define PF2                     (*((volatile unsigned long *)0x40025010))
#define PF1                     (*((volatile unsigned long *)0x40025008))
#define PF0                     (*((volatile unsigned long *)0x40025004))
#define PF31                    (*((volatile unsigned long *)0x40025028)) // pf3 & pf1
#define GPIO_PORTF_DR2R_R       (*((volatile unsigned long *)0x40025500))
#define GPIO_PORTF_DR4R_R       (*((volatile unsigned long *)0x40025504))
#define GPIO_PORTF_DR8R_R       (*((volatile unsigned long *)0x40025508))
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

#define	allStop	  0 // All traffic red and no pedestrians
#define	goSouth	  1 // South light green
#define	slowSouth	2 // West light yellow
#define	goWest	  3 // West light green
#define	slowWest	4 // South light yellow
#define	walk	    5 // Pedestrian Walk
#define	hurry1	  6 // don't walk flash cycle
#define	hurry2	  7
#define	hurry3	  8
#define	hurry4	  9
#define	cycle1	 10 // round robin 111 input
#define	cycle2	 11
#define	cycle3	 12
#define	cycle4	 13
	
unsigned long S;  // index to the current state 
unsigned long Input; 
unsigned long WalkGreen; 
unsigned long WalkRed; 
typedef const struct State StateType;

// Linked Data Structure
struct State {
  unsigned long OutTraffic; 
	unsigned long OutWalk;
  unsigned long Time;
  unsigned long Next[8];
};

// Data structure that captures the FSM state transition graph
StateType FSM[14] = {
	{0x24,0x02,ALLSTOP_DELAY,{walk,goWest,goSouth,goWest,walk,walk,walk,cycle1}},
	{0x21,0x02,GREEN_DELAY, {goSouth,slowSouth,goSouth,slowSouth,slowSouth,slowSouth,slowSouth,cycle1}},
	{0x22,0x02,YELLOW_DELAY, {allStop,goWest,allStop,goWest,walk,walk,walk,cycle3}},
	{0x0C,0x02,GREEN_DELAY, {allStop,goWest,slowWest,slowWest,slowWest,slowWest,slowWest,cycle1}},
	{0x14,0x02,YELLOW_DELAY,{allStop,allStop,goSouth,goSouth,walk,walk,walk,cycle1}},
	{0x24,0x01,GREEN_DELAY,{walk,hurry1,hurry1,hurry1,walk,hurry1,hurry1,hurry1}},
	{0x24,0x02,HURRY_DELAY,{hurry2,hurry2,hurry2,hurry2,hurry2,hurry2,hurry2,hurry2}},
	{0x24,0x00,HURRY_DELAY,{hurry3,hurry3,hurry3,hurry3,hurry3,hurry3,hurry3,hurry3}},
	{0x24,0x02,HURRY_DELAY,{hurry4,hurry4,hurry4,hurry4,hurry4,hurry4,hurry4,hurry4}},
	{0x24,0x00,HURRY_DELAY,{allStop,goWest,goSouth,goSouth,walk,goWest,goSouth,cycle1}},
	{0x21,0x02,GREEN_DELAY,{goSouth,slowSouth,goSouth,slowSouth,slowSouth,slowSouth,slowSouth,cycle2}},
	{0x22,0x02,YELLOW_DELAY,{allStop,goWest,allStop,goWest,walk,walk,walk,cycle3}},
	{0x0C,0x02,GREEN_DELAY,{goWest,goWest,slowWest,goSouth,slowWest,slowWest,slowWest,cycle4}},
	{0x14,0x02,YELLOW_DELAY,{allStop,allStop,goSouth,goSouth,walk,walk,walk,walk}}
};


// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void); // Port E initialization
void PLL_Init(void); 

// ***** 3. Subroutines Section *****

void PortB_E_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x12;      // 1) B E
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0
	
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
}


// Walk LED

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}


// SysTick

#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
	
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}

// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}


 int main(void){ volatile unsigned long delay;
  PLL_Init();       // 80 MHz, Program 10.1
  SysTick_Init();   // Program 10.2
	PortF_Init();
	PortB_E_Init(); // Sensors and lights
	 
  S = goSouth; // initial state

	 while(1){
    TRAFFICLIGHT = FSM[S].OutTraffic;  // set traffic lights first (requirement)
	  WalkRed = (FSM[S].OutWalk >> 1) * RED;
	  WalkGreen = (FSM[S].OutWalk) * GREEN;
		PF31 = WalkRed + WalkGreen;
    SysTick_Wait10ms(FSM[S].Time);
    Input = SENSOR;     // read sensors
    S = FSM[S].Next[Input];  
  }
}
 