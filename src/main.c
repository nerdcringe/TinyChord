#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bitwise.h"
#include "pwm.h"
#include "synth.h"
#include "adc.h"
#include "mymath.h"

#define RAD_TO_REV 0.159155

#define MINOR_BTN PB0

#define A 440
#define As 466
#define B 494
#define C 523
#define Cs 554
#define D 587
#define Ds 622
#define E 660
#define F 698
#define Fs 740
#define G 784
#define Gs 831

// Equivalent sharps and flats
#define Ab Gs
#define Bb As
#define Db Cs
#define Eb Ds
#define Gb Fs

int main () {
	const uint8_t NUM_OCTAVES = 5;
	const int STRUM_SIZE = 255/NUM_OCTAVES;

	int xPotVal = 0;
	int yPotVal = 0;
	int strumPotVal = 0;

	uint16_t chords[7][3] = {
		{A,	Cs, E},
		{B, D, Fs},
		{Cs, E, Gs},
		{D, Fs, A*2},
		{E, Gs, B*2},
		{Fs, A*2, Cs*2},
		{Gs, B*2, D*2},
	};

	
	uint16_t chords2[12][3] = {
		{C, E, G},
		{G, B, D},
		{D, Fs, A},
		{A, Cs, E},
		{E, Gs, B},
		{B, Ds, Fs},
		{Gb, Bb, Db},
		{Db, F, Ab},
		{Ab, C, Eb},
		{Eb, G, Bb},
		{Bb, D, F},
		{F, A, C},
	};

	uint16_t notes[] = {
		440, 494, 554, 587, 660, 740, 831
	};

	// set outputs, clear inputs
	DDRB = clearBit(DDRB, MINOR_BTN); // set data direction to input
	PORTB = setBit(PORTB, MINOR_BTN); // turn on port to enable pullup resistor
	initPWM();
	initSynth();
	initADC();

	while (1) {
		xPotVal = readADC3(); // use ADC3
		xPotVal += -2-128;
		yPotVal = readADC2(); // use ADC2
		yPotVal += 3-128;

		strumPotVal = readADC1();

		uint8_t minorPress = checkBit(PORTB, MINOR_BTN);
		uint8_t settingPress = readADC0() < 230;

		float angle = atan2f(yPotVal, xPotVal) * RAD_TO_REV;
		int chordIndex = 0;
		int noteFreq = 0;
		noteFreq = chords2[chordIndex][0];
		
		/*
		if (fabs(angle) < 0.4488) {
			noteFreq = notes[0];
		} else if (angle > 0) {
			if (angle < 1.346) {
				noteFreq = notes[1];
			} else if (angle < 2.244) {
				noteFreq = notes[2];
			} else {
				noteFreq = notes[3];
			}
		} else {
			if (angle > -1.346) {
				noteFreq = notes[6];
			} else if (angle > -2.244) {
				noteFreq = notes[5];
			} else {
				noteFreq = notes[4];
			}
		}*/
		
		int distance = hypotf(yPotVal, xPotVal);
		if (distance > 6) {
			enableSynth();
			/*if (distance < 53) {
				noteFreq /= 2;
			} else if (distance > 123) {
				noteFreq *= 2;
			}*/
		} else {
			disableSynth();
			noteFreq = 0;
		}

		for (int i = 0; i < 256; i += STRUM_SIZE) {
			if (strumPotVal < i) {
				noteFreq *= 2;
			}
		}
		
		setJump(noteFreq/2);
	}
}