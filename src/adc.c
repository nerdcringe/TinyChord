#include "adc.h"
#include <avr/io.h>
#include <util/delay.h>

// Delay between storing and returning analog value (in microseconds)
#define READ_DELAY_US 100

void initADC() {
	// ADLAR = 1 (Left adjust ADCH register)
	// MUX[3:0] = 010 (Start with ADC2 as input channel)
	ADMUX |= (1<<ADLAR) | (1<<MUX1);
	// ADEN = 1 (Enable the ADC)
	ADCSRA |= 1<<ADEN;
	// ADPS[2:0] = 011 (Set 1/8 prescaling for ADC clock)
	ADCSRA |= (1<<ADPS1) | (1<<ADPS0);
	
}

// Read analog pin 0 (PB5)
uint8_t readADC0() {
	// ADMUX[1:0] = 00 (ADC0)
	ADMUX &= ~(1<<MUX1);
	ADMUX &= ~(1<<MUX0);
	ADCSRA |= 1<<ADSC; // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}

// Read analog pin 1 (PB2)
uint8_t readADC1() {
	// ADMUX[1:0] = 01 (ADC1)
	ADMUX &= ~(1<<MUX1);
	ADMUX |= (1<<MUX0);
	ADCSRA |= 1<<ADSC; // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}

// Read analog pin 2 (PB4)
uint8_t readADC2() {
	// ADMUX[1:0] = 10 (ADC2)
	ADMUX |= (1<<MUX1);
	ADMUX &= ~(1<<MUX0);
	ADCSRA |= 1<<ADSC; // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}

// Read analog pin 3 (PB3)
uint8_t readADC3() {
	// ADMUX[1:0] = 11 (ADC3)
	ADMUX |= (1<<MUX1);
	ADMUX |= (1<<MUX0);
	ADCSRA |= 1<<ADSC; // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}