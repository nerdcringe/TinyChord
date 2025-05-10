#include "synth.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define NUM_VOICES 2

/* Start timer0 interrupts and enable synthesis */
void initSynth() {
	disableSynth();
	// Set up Timer/Counter1 for PWM output
	// Register settings from http://www.technoblogy.com/show?20W6

 	// Timer interrupts OFF
	TIMSK = 0;
	//Enable 64 MHz PLL and use as source for Timer1
	PLLCSR = 1<<PCKE | 1<<PLLE;
	// PWM1A = 1 (PWM on comparator OCR1A, clear on match)
	TCCR1 |= 1<<PWM1A;
	// COM1A[1:0] = 10
	TCCR1 |= 1<<COM1A1;
	// CS1[3:0] = 0001 (1:1 prescaling)
	TCCR1 |= 1<<CS10;
	// set data direction for pwm pin to output
	DDRB |= 1<<DDB1;

	// WGM0[2:0] = 111 (Fast PWM, Enable compare match, disable overflow)
	TCCR0A |= (1<<WGM02) | (1<<WGM01) | (1<<WGM00);
	//CS0[2:0] = 001 (1:1 prescaling)
	TCCR0B |= 1<<CS00;
	//OCIE1A = 1 (Enable compare match, disable overflow)
	TIMSK |= 1<<OCIE0A;
	// Divide by 61, giving us increments of about 1 Hz
	OCR0A = 60;
	enableSynth();
}

/* Disables synth output*/
void disableSynth() {
	// Intellisense may see an error on cli() due to assembly code but it compiles 
	cli();
	OCR1A = 0;
	TCCR1 &= ~(1<<CS10);
	PORTB &= ~(1<<PB1);
}

/* Enables synth after it has been disabled (enabled by initSynth) */
void enableSynth() {
	DDRB |= (1<<PB1);
	TCCR1 |= 1<<CS10;
	sei(); 
}

// Phase accumulator: keeps track of current phase of wave
volatile uint16_t accumulator[NUM_VOICES] = {0};

 // How far to increment the corresponding phase accumulator
volatile uint32_t jump[NUM_VOICES] = {0};

void setJump(uint8_t jumpIndex, uint32_t newJump) {
	jump[jumpIndex] = newJump;
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

// Assembly may cause red underline in intellisense
ISR(TIMER0_COMPA_vect) {
	uint16_t sample = 0;
	for (int i = 0; i < NUM_VOICES; i++) {
		accumulator[i] += jump[i];
		uint8_t accum8Bit = (uint8_t)(accumulator[i] >> 8);
		sample += sineLUT[accum8Bit/2];
	}
	OCR1A = (uint8_t)(sample/NUM_VOICES);
}
