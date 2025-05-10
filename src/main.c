#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "synth.h"
#include "adc.h"
#include "theory.h"

#define CHECK_BIT(var,pos) (((var) & (1<<(pos))) > 0)

#define BTN_0 PB4
#define BTN_1 PB3
#define BTN_2 PB2
#define MINOR_BTN PB0


int main () {
	const uint8_t NUM_OCTAVES = 4;
	const int STRUM_SIZE = 255/NUM_OCTAVES;

	int strumPotVal = 0;

	uint16_t chords[8][3] = {
		{A,	Cs, E},
		{B, D, Fs},
		{Cs, E, Gs},
		{D, Fs, A*2},
		{E, Gs, B*2},
		{Fs, A*2, Cs*2},
		{Gs, B*2, D*2},
	};
	uint16_t notes[] = { A, B, Cs, D, E, Fs, Gs };

	// set outputs, clear inputs
	/*DDRB &= ~(1<<MINOR_BTN); // set data direction to input
	PORTB |= 1<<MINOR_BTN; // turn on port to enable pullup resistor
	*/

	// PULLUP RESISTORS
	PORTB |= (1<<BTN_0);
	PORTB |= (1<<BTN_1);
	PORTB |= (1<<BTN_2);

	initSynth();
	//initADC();
	int noteFreq = 440;
	int noteFreq2 = 440;
	uint8_t minorPress;
	
	while (1) {
		//int buttonVal = readADC0();
		//minorPress = CHECK_BIT(PINB, MINOR_BTN);
		uint8_t numPressed = !CHECK_BIT(PINB, BTN_0) << 2 |
							!CHECK_BIT(PINB, BTN_1) << 1 |
							!CHECK_BIT(PINB, BTN_2);
		if (numPressed == 0) {
			disableSynth();
			noteFreq = 0;
		} else {
			enableSynth();
			noteFreq = chords[(numPressed-1) % 7][0];
			noteFreq2 = chords[(numPressed+1) % 7][0];
		}
/*
		for (int i = 0; i < 256; i += STRUM_SIZE) {
			if (strumPotVal-32 < i) {
				noteFreq *= 2;
			}
		}*/
		setJump(0, noteFreq*16);
		setJump(1, noteFreq2*16);
		//setJump(0, 440*8);
		//setJump(1, 660*8);
		//setJump(440*8);
	}
}