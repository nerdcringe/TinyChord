#include "synth.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm.h"

/* Start timer0 interrupts and enable synthesis */
void initSynth() {
	disableSynth();
	// Set up Timer/Counter0 for sample interrupt
	TCCR0A = 3<<WGM00;                        // Fast PWM
	TCCR0B = 1<<WGM02 | 1<<CS00;             // No prescale
	TIMSK = 1<<OCIE0A;                       // Enable compare match, disable overflow
	OCR0A = 121;                              // Divide by 61, giving us increments of about 1 Hz
	enableSynth(); // allow interrupts
}

/* Disables synth output*/
void disableSynth() {
	// The IDE may see an error on cli() due to assembly code but it compiles 
	cli();
	setDutyCycle(0);
}

/* Enables synth after it has been disabled (enabled by initSynth) */
void enableSynth() {
	sei();
}

// Phase accumulator: keeps track of current phase of wave
volatile uint16_t accumulator = 0;

 // How far to increment the corresponding phase accumulator
volatile uint16_t jump = 440;

void setJump(uint16_t newJump) {
	jump = newJump;
}

#define LUT_SIZE 128

const uint8_t sineLUT[LUT_SIZE] = {
	128, 134, 140, 146, 152, 158, 165, 170, 176, 182, 188, 193, 198, 203, 208, 213,
	218, 222, 226, 230, 234, 237, 240, 243, 245, 248, 250, 251, 253, 254, 254, 255,
	255, 255, 254, 254, 253, 251, 250, 248, 245, 243, 240, 237, 234, 230, 226, 222,
	218, 213, 208, 203, 198, 193, 188, 182, 176, 170, 165, 158, 152, 146, 140, 134,
	128, 121, 115, 109, 103, 97, 90, 85, 79, 73, 67, 62, 57, 52, 47, 42,
	37, 33, 29, 25, 21, 18, 15, 12, 10, 7, 5, 4, 2, 1, 1, 0,
	0, 0, 1, 1, 2, 4, 5, 7, 10, 12, 15, 18, 21, 25, 29, 33,
	37, 42, 47, 52, 57, 62, 67, 73, 79, 85, 90, 97, 103, 109, 115, 121
};

ISR(TIMER0_COMPA_vect) {
	accumulator += jump;
	uint8_t accum8Bit = accumulator >> 8;
	uint8_t sample = 255;
	sample = sineLUT[accum8Bit/2];
	setDutyCycle(sample);
}
