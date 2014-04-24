// Sound.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// March 13, 2014

// Period =  (80000000/16)/Freq = 5000000/Freq
#define C1   2389   // 2093 Hz
#define B1   2531   // 1975.5 Hz
#define BF1  2681   // 1864.7 Hz
#define A1   2841   // 1760 Hz
#define C0   9556   // Piano key 0: C (523.251 Hz)
#define D    8513   // Piano key 1: D (587.330 Hz)
#define E    7584   // Piano key 2: E (659.255 Hz)
#define G    6378   // Piano key 3: G (783.991 Hz)


void DAC_Init(void);

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also initializes DAC
// Input: none
// Output: none
void Sound_Init(unsigned long period);

// **************Sound_Tone*********************
// Change SysTick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period);


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void);

