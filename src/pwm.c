#include "pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "bitwise.h"

// DO NOT CHANGE: pwm is configured differently for each pin
#define PWM_PIN PB1

void initPWM() {
	// Set up Timer/Counter1 for PWM output
	// Register settings from http://www.technoblogy.com/show?20W6
	// PWM1A = 1 (PWM on comparator OCR1A, clear on match)
	TCCR1 = setBit(TCCR1, PWM1A);
	// COM1A[1:0] = 10
	TCCR1 = setBit(TCCR1, COM1A1);
	// CS1[3:0] = 001 (1:1 prescaling)
	TCCR1 = setBit(TCCR1, CS10);

	// set data direction for pwm pin to output
	DDRB = setBit(DDRB, PWM_PIN);
}

/* Set the duty cycle of PWM from 0-255 */
void setDutyCycle(uint8_t dutyCycle) {
	OCR1A = dutyCycle;
}
