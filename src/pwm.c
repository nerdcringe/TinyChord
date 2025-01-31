#include "pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "bitwise.h"

// DO NOT CHANGE: pwm is configured differently for each pin
#define PWM_PIN PB1

void initPWM() {
	// Set up Timer/Counter1 for PWM output
	TIMSK = 0;                               // Timer interrupts OFF
	TCCR1 = 1<<PWM1A | 2<<COM1A0 | 1<<CS10;  // PWM A, clear on match, 1:1 prescale

	DDRB |= 1<<PWM_PIN; // enable output on pwm pin
}

/* Set the duty cycle of PWM from 0-255 */
void setDutyCycle(uint8_t dutyCycle) {
	OCR1A = dutyCycle;
}
