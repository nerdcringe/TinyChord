#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "synth.h"
#include "adc.h"
#include "SN74HC165.h"
#include "theory.h"

#define CHECK_BIT(var,pos) (((var) & (1<<(pos))) > 0)


/* SN74HC165 PARALLEL-IN SERIAL-OUT SHIFT REGISTER */
// Connections to uC
#define DATA PB3 // Serial Data Output
#define CLK PB2 // Shift Clock
#define LD PB0 // Parallel Load

/* SHIFT REGISTER INPUT PINS */
// Chord Buttons: Click or hold in certain combination to play a certain chord
#define BTN_0 5 //
#define BTN_1 6
#define BTN_2 7
// Modifier Buttons: Hold down to modify the chord
#define BTN_MAJ_MIN 4
#define BTN_DIM	3
#define BTN_FLAT7 2
// Settings Buttons: Click once to change a certain setting
#define BTN_KEY 1
#define BTN_WAVE 0

/* STRUMMING */
// When the stylus is on the graphite strip, it reads a certain analog value (strum value).
// This will change the inversion of the chord, making the notes higher or lower pitch
// When the stylus is removed, then a pullup resistor pulls it HIGH.

#define STRUM_THRESHOLD 240 // Assume the stylus is removed when analog value is above this
#define NUM_INVERSIONS 12 // Total number of inversions
#define FREQ_SCALE 2 // Scale up by a power of 2 to raise the octave


int main () {
	const int STRUM_SIZE = 256/NUM_INVERSIONS;
	uint8_t incoming = 0; // Incoming bits from shift register
	uint8_t btnCode = 0; // Binary representation of button state
	uint8_t lastBtnCode = 0;
	uint16_t currChord = 1; // Current chord number (1: I, 2: ii, 3: iii, 4: IV, etc...)
	uint32_t currFreqs[MAX_FREQS] = {440, 440, 440, 440};
	
	uint16_t strumValue = 255; // Analog input value from stylus (determines inversions)
	uint16_t lastStrumValue = 96; // Analog input value when stylus was last touching
	uint8_t keyOffset = 0; // # Steps to shift key
	uint8_t waveType = 0; // Determine waveform type
	bool keyBtnPressed = false;
	bool waveBtnPressed = false;
	
	initInputRegister(8, DATA, CLK, LD);
	initSynth();
	initADC();

	while (1) {
		incoming = readInputRegister();
		strumValue = readADC2();

		/* Turn the button state into a 3-bit binary code
			Each bit corresponds to one of 3 buttons being pressed
			In base ten it gives the chord number
			001: I chord
			010: ii chord
			011: iii chord
			100: IV chord, etc...
			*/
		btnCode = CHECK_BIT(incoming, BTN_0) << 2 |
					CHECK_BIT(incoming, BTN_1) << 1 |
					CHECK_BIT(incoming, BTN_2);
		
		if (strumValue > STRUM_THRESHOLD && btnCode == 0) {
			disableSynth();
		} else {
				// If stylus is not touching, update chord continuously
				// You must keep buttons held to play
			if ((strumValue > STRUM_THRESHOLD && btnCode != 0) ||
				// If stylus is touching, don't update chord when button is released
				// You can tap the buttons once and strum the same chord without
				// worrying about releasing all buttons at the exact same time
			btnCode > lastBtnCode) {
				// Update the chord based on the binary number pressed
				currChord = btnCode;
			}
			enableSynth();
		}
		
		// Find each frequency of the chord and put it in currFreqs array
		for (uint8_t i = 0; i < MAX_FREQS; i++) { 
 			// Find # steps above the key center
			uint8_t stepWithinKey = chordsNotes[currChord-1][i];

			if (i == 1 && CHECK_BIT(incoming, BTN_MAJ_MIN)) {
				// Turn major chords minor
				if (currChord == 1 || currChord == 4 || currChord == 5) {
					stepWithinKey -= 1;
				} else {
					// Turn minor chords major
					stepWithinKey += 1;
				}
			}

			if (i == 3 && CHECK_BIT(incoming, BTN_FLAT7)) {
				if (currChord == 1 || currChord == 4 || currChord == 5 || currChord == 7) {
					stepWithinKey -= 1;
				} else {
					stepWithinKey += 1;
				}
			}

			// Find the note by shifting based on the key center
			uint8_t freqIndex = stepWithinKey + keyOffset;
			// Convert note to frequency and scale to change the octave
			currFreqs[i] = noteToFreq[freqIndex % 12] * FREQ_SCALE;
		}

		// Start at high octave and invert (lower the highest note) recursively based on strum value
		for (int i = 0; i <= lastStrumValue; i += STRUM_SIZE) {
			// Find the highest frequency in the chord
			uint8_t highestFreqIndex = 0;
			uint16_t highestFreq = 0;
			for (int j = 0; j < MAX_FREQS; j++) {
				if (currFreqs[j] > highestFreq) {
					highestFreq = currFreqs[j];
					highestFreqIndex = j;
				}
			}
			// Divide highest frequency by 2 to bring it down an octave
			currFreqs[highestFreqIndex] /= 2;
		}

		// Change the rate that the samples are played to the calculated frequencies
		for (int i = 0; i < MAX_FREQS; i++) {
			setJump(i, currFreqs[i]);
		}

		// Change key when the key button is first pressed
		if (CHECK_BIT(incoming, BTN_KEY)) {
			if (!keyBtnPressed) {
				keyOffset = (keyOffset+1) % 12;
				keyBtnPressed = true;
			}
		} else {
			keyBtnPressed = false;
		}

		// Change the wave type when the wave button is first pressed
		if (CHECK_BIT(incoming, BTN_WAVE)) {
			if (!waveBtnPressed) {
				waveType = (waveType+1) % NUM_WAVE_TYPES;
				setWaveType(waveType);
				waveBtnPressed = true;
			}
		} else {
			waveBtnPressed = false;
		}
		
		// Remember the last button code pressed
		lastBtnCode = btnCode;

		// Remember the strum value when the stylus was touching last
		if (strumValue < STRUM_THRESHOLD) {
			lastStrumValue = strumValue;
		}
	}
}