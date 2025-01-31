#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bitwise.h"
#include "pwm.h"
#include "synth.h"
#include "adc.h"
#include "mymath.h"

#define JOY_BTN PB0

int main () {
	int pot1Val = 0;
	int pot2Val = 0;

	uint16_t notes[] = {
		440, 494, 554, 587, 660, 740, 831
	};

	// set outputs, clear inputs
	DDRB = clearBit(DDRB, JOY_BTN); // set data direction to input
	PORTB = setBit(PORTB, JOY_BTN); // turn on port to enable pullup resistor
	initPWM();
	initSynth();
	initADC();

	while (1) {
		pot1Val = readADC3(); // use ADC3 (pot 1)
		pot1Val += -2-128;
		pot2Val = readADC2(); // use ADC2 (pot 2)
		pot2Val += 3-128;

		float angle = atan2f(pot2Val, pot1Val);
		int noteFreq = 0;

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
		}

		int distance = hypotf(pot2Val, pot1Val);
		if (distance > 6) {
			enableSynth();
			if (distance < 53) {
				noteFreq /= 2;
			} else if (distance > 123) {
				noteFreq *= 2;
			}
		} else {
			disableSynth();
			noteFreq = 0;
		}
		setJump(noteFreq);
	}
}