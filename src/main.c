#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "synth.h"
#include "adc.h"
#include "theory.h"

#define CHECK_BIT(var,pos) (((var) & (1<<(pos))) > 0)

#define BTN_0 PB3
#define BTN_1 PB2
#define BTN_2 PB0
//#define MINOR_BTN PB0

#define STRUM_OFFSET 32
#define STRUM_THRESHOLD 200
#define NUM_INVERSIONS 18
#define FREQ_SCALE 4

int main () {
	const int STRUM_SIZE = 256/NUM_INVERSIONS;
	uint32_t topFreq = 440;
	uint32_t bottomFreq = 440;
	uint32_t midFreq = 440;
	
	uint8_t minorPress = 0;
	uint16_t currChord = 0;
	uint8_t numPressed = 0;
	uint8_t lastNumPressed = 0;

	uint16_t strumVal = 255;
	uint16_t lastStrumVal = 127;

	uint16_t chords[8][3] = {
		{A,	Cs, E}, //A
		{B, D, Fs}, //Bm
		{Cs, E, Gs}, //C#m
		{D, Fs, A*2}, //D
		{E, Gs, B*2}, //E
		{Fs, A*2, Cs*2}, //F#m
		{Gs, B*2, D*2}, //Gmb5
	};
	uint16_t altChords[8][3] = {
		{A,	C, E}, //Am
		{B, Ds, Fs}, //B
		{Cs, F, Gs}, //C#
		{D, F, A*2}, //Dm
		{E, G, B*2}, //Em
		{Fs, As*2, Cs*2}, //F#
		{G, C*2, D*2}, //G
	};

	// set outputs, clear inputs
	/*DDRB &= ~(1<<MINOR_BTN); // set data direction to input
	PORTB |= 1<<MINOR_BTN; // turn on port to enable pullup resistor
	*/

	// PULLUP RESISTORS
	PORTB |= (1<<BTN_0);
	PORTB |= (1<<BTN_1);
	PORTB |= (1<<BTN_2);
	//PORTB |= (1<<MINOR_BTN);
	initSynth();
	initADC();

	while (1) {
		strumVal = readADC2();
		/*if (strumPotVal <= STRUM_OFFSET) {
			strumPotVal = 0;
		} else {
			strumPotVal -= STRUM_OFFSET;
		}*/
		//minorPress = CHECK_BIT(PINB, MINOR_BTN);
		numPressed = !CHECK_BIT(PINB, BTN_0) << 2 |
						!CHECK_BIT(PINB, BTN_1) << 1 |
						!CHECK_BIT(PINB, BTN_2);
		if (strumVal > STRUM_THRESHOLD && numPressed == 0) {
			disableSynth();
		} else {
			if (numPressed > lastNumPressed || (strumVal > STRUM_THRESHOLD && numPressed > 0)) {
				currChord = (numPressed-1) % 7;
			}
			enableSynth();
		}
	
		topFreq = chords[currChord][0] * FREQ_SCALE*2;
		midFreq = chords[currChord][2] * FREQ_SCALE;
		bottomFreq = chords[currChord][1] * FREQ_SCALE;
		/*if (minorPress) {
			noteFreq2 = chords[(numPressed-1) % 7][1];
		} else {
			noteFreq2 = chords[(numPressed-1) % 7][2];
		}*/
		//int lowTonic = topFreq/16;
		for (int i = 0; i < lastStrumVal/STRUM_SIZE; i++) {
			if (i % 3 == 0) {
				topFreq /= 2;
			} else if (i % 3 == 1) {
				midFreq /= 2;
			} else if (i % 3 == 2) {
				bottomFreq /= 2;
			}	
		}
		setJump(0, topFreq);
		setJump(1, bottomFreq);
		setJump(2, midFreq);
		//setJump(3, lowTonic);
		//setJump(0, 440*8);
		//setJump(1, 660*8);
		//setJump(440*8);
		lastNumPressed = numPressed;
		if (strumVal < STRUM_THRESHOLD) {
			lastStrumVal = strumVal;
		}
	}
}