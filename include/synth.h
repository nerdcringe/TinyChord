#include <avr/io.h>

// Number of frequencies to play at once
// Can only do 5 or so until interrupt takes too long and breaks
// (interrupt takes longer than the time between interrupts)
#define MAX_FREQS 4

// Define enum for wave types
#define NUM_WAVE_TYPES 4
#define SINE 3
#define TRIANGLE 0
#define SAWTOOTH 1
#define SQUARE 2


void initSynth(); // Initialize synth module
void disableSynth(); // Turn off synth temporarily
void enableSynth(); // Start synth again after disabling
void setWaveType(uint8_t waveType);
void setJump(uint8_t jumpIndex, uint32_t newJump); // Set speed to play the sample
