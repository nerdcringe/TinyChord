#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "synth.h"
#include "adc.h"
#include "encoder.h"
#include "SN74HC165.h"
#include "theory.h"


/* TinyChord Synthesizer

FEATURES
- ATtiny85 based direct digital synthesizer
- Plays four-note chords from the traditional western music scale
- Outputs pulse-width modulated (PWM) audio signal from digital output
- Buttons hooked up to parallel-in serial-out shift register (SN74HC165)

CHORD BUTTONS
- Press chord buttons in different combinations to play different chords
- Based on (reversed) binary code (left to right):
	- 100 = I
	- 010 = ii
	- 110 = iii
	- 001 = IV
	- 101 = V
	- 011 = vi
	- 111 = vii (b5)

PEDAL/PLAY BUTTON
- Hold down to actually play the note
- Connects capacitor from the PWM output to a fixed reference
	- Otherwise the sound fades out (floating capacitor)
	- Release time is controllable with potentiometer
	- No associated code (purely analog)

OTHER BUTTONS
- Change key (only goes up but loops around)
- Select waveform (triangle, sawtooth, square, sine)
- Change chord intervals
	- Swap major and minor 3rds
	- Swap major and minor 7ths
	- Diminish by raising root by a semitone

ENCODER
- Rotate to "strum" for expression
- Cycles through different inversions in multiple octaves

*/

#define CHECK_BIT(var,pos) (((var) & (1<<(pos))) > 0)


/* SHIFT REGISTER CONNECTIONS
	Connect directly to microcontroller */
//#define DATA PB3 // Serial Data Output (REPLACED BY ADC0, SEE SN74HC165.C!!!)
#define CLK PB2 // Shift Clock (CP)
#define LD PB0 // Parallel Load (/PL)

/* SHIFT REGISTER INPUT PINS
	Connect to buttons (needs external pulldowns )*/

// Chord Buttons: Click or hold in certain combination to play a certain chord
#define BTN_CHORD_0 5 //
#define BTN_CHORD_1 6
#define BTN_CHORD_2 7
// Interval Buttons: Hold down to change the chord invervals
#define BTN_SWAP_3RD 4 // Swap between major and minor 3rd
#define BTN_SWAP_7TH 3 // Swap between major and minor 7th
#define BTN_DIM 2
// Settings Buttons: Click once to change a certain setting
#define BTN_KEY 1 // Change key
#define BTN_WAVE 0 // Change wave type

/* ENCODER */
// Needs two inputs to measure the direction of rotation
#define ENCODER_A PB3 // Encoder might say "CLK"
#define ENCODER_B PB4 // May be "DT"

#define NUM_INVERSIONS 12 // Maximum number of inversions allowed (4 per octave)
#define OCTAVE_OFFSET 1 // Notes are offset by certain # octaves
#define FREQ_SCALE 1<<OCTAVE_OFFSET // Each octave scales frequency by 2


int main () {
	uint8_t incoming = 0; // Incoming bits from shift register
	uint8_t chordBtnState = 0; // Binary representation of which chord buttons are pressed
	uint8_t lastChordBtnState = 0;
	uint16_t currChord = 1; // Current chord number (1 = I, 2 = ii, 3 = iii, etc...)
	uint32_t currFreqs[MAX_FREQS] = {440, 440, 440, 440}; // Frequencies that make up the current chord
	int8_t currInversion = 4; // # Times to invert chord by lowering top note
	uint8_t keyOffset = 0; // # Steps to shift key
	uint8_t waveType = 0; // Determine waveform type
	
	bool moodSwap = false;
	bool diminish = false;
	bool moodSwapPressed = false;
	bool diminishPressed = false;
	bool keyBtnPressed = false;
	bool waveBtnPressed = false;
	
	initInputRegister(8, CLK, LD);
	initEncoder(ENCODER_A, ENCODER_B);
	initSynth();
	initADC();
	setEncoderValue(currInversion*2);

	while (1) {
		// Read incoming bits for buttons
		incoming = readInputRegister();
		
		/* Turn the button state into a 3-bit binary code
		Each bit corresponds to one of 3 buttons being pressed
		In base ten it gives the chord number */
		chordBtnState = CHECK_BIT(incoming, BTN_CHORD_0) << 2 |
				CHECK_BIT(incoming, BTN_CHORD_1) << 1 |
				CHECK_BIT(incoming, BTN_CHORD_2);

		/* Update btnCode only when a new button is pressed
			Don't update when releasing a button so you don't have to keep them pressed */
		if (chordBtnState > lastChordBtnState) {
			currChord = chordBtnState;

			// Reset interval buttons when new chord is pressed
			moodSwap = false;
			diminish = false;
		}
		
		
		// Find each frequency of the chord and put it in currFreqs array
		for (uint8_t i = 0; i < MAX_FREQS; i++) {
 			// Find # steps above the key center
			uint8_t stepWithinKey = 0;

			// Choose which scale of notes to use
			if (moodSwap) {
				stepWithinKey = moodSwapNotes[currChord-1][i];
			} else if (diminish) {
				stepWithinKey = diminishedNotes[currChord-1][i];
			} else {
				stepWithinKey = defaultNotes[currChord-1][i];
			}
			
			// Find the note by shifting based on the key center
			uint8_t freqIndex = stepWithinKey + keyOffset;
			// Convert note to frequency and scale to change the octave
			currFreqs[i] = noteToFreq[freqIndex % 12] * FREQ_SCALE;
		}
		
		// Change # times the chord is inverted based on encoder position
		int8_t currEncoderValue = getEncoderValue();

		// Limit encoder value between 0 and NUM_INVERSIONS
		// An encoder click changes value by 2 so compensate for that
		if (currEncoderValue > NUM_INVERSIONS*2) {
			currEncoderValue = NUM_INVERSIONS*2;
		} else if (currEncoderValue < 0) {
			currEncoderValue = 0;
		}
		// Update when fully clicked into next slot
		currInversion = currEncoderValue/2;

		// Update the constrainted encoder value so it stays in range
		setEncoderValue(currEncoderValue);

		// Invert based on current # inversions
		// Start at high octave and lower the highest note repeatedly
		for (int i = 0; i < currInversion; i++) {
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

		// Check if interval buttons are pressed
		// Keep until new chord is pressed
		if (CHECK_BIT(incoming, BTN_SWAP_3RD)) {
			if (!moodSwapPressed) {
				moodSwapPressed = true;
				moodSwap = !moodSwap;
				diminish = false;
			}
		} else {
			moodSwapPressed = false;
		}

		if (CHECK_BIT(incoming, BTN_DIM)) {
			if (!diminishPressed) {
				diminishPressed = true;
				diminish = !diminish;
				moodSwap = false;
			}
		} else {
			diminishPressed = false;
		}

		// Change key when the key button is first pressed
		if (CHECK_BIT(incoming, BTN_KEY)) {
			if (!keyBtnPressed) {
				// Raise offset and keep in range (12 keys total)
				keyOffset = (keyOffset+1) % 12;
				keyBtnPressed = true;
			}
		} else {
			keyBtnPressed = false;
		}

		// Change the wave type when the wave button is first pressed
		if (CHECK_BIT(incoming, BTN_WAVE)) {
			if (!waveBtnPressed) {
				// Go to next wave type and keep in range
				waveType = (waveType+1) % NUM_WAVE_TYPES;
				setWaveType(waveType);
				waveBtnPressed = true;
			}
		} else {
			waveBtnPressed = false;
		}
		
		// Keep track to know when chord button state changes
		lastChordBtnState = chordBtnState;
	}
}