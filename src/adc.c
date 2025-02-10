#include "adc.h"
#include <avr/io.h>
#include <util/delay.h>
#include "bitwise.h"

#define READ_DELAY_US 100

void initADC() {
	/* This tutorial shows how to read analog values without the Arduino library.
		https://www.instructables.com/ATTiny-Port-Manipulation-Part-2-AnalogRead/
	*/

	// Set up ADC input for the potentiometers
	ADMUX = 0b00100010;   //sets Vcc reference, sets ADC2 as input channel, and left adjusts (8 MSBs in ADCH)
	ADCSRA = 0b10000011;  //turn on ADC, keep ADC single conversion mode,
						  //and set division factor-8 for 125kHz ADC clock

	/*
	// ADLAR = 1 (Left adjust ADCH register)
	ADMUX = setBit(ADMUX, ADLAR);
	// MUX[3:0] = 010 (Start with ADC2 as input channel)
	ADMUX = setBit(ADMUX, MUX1);

	// ADEN = 1 (Enable the ADC)
	ADCSRA = setBit(ADCSRA, ADEN);

	// ADPS[2:0] = 011 (Set 1/8 prescaling for ADC clock)
	ADCSRA = setBit(ADCSRA, ADPS1);
	ADCSRA = setBit(ADCSRA, ADPS0);
	*/
}

uint8_t readADC0() {
	// ADMUX[1:0] = 00 (ADC0)
	ADMUX = clearBit(ADMUX, MUX1);
	// Set the active adc
	ADMUX = clearBit(ADMUX, MUX0);
	ADCSRA = setBit(ADCSRA, ADSC); // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}

uint8_t readADC1() {
	// ADMUX[1:0] = 01 (ADC1)
	ADMUX = clearBit(ADMUX, MUX1);
	// Set the active adc
	ADMUX = setBit(ADMUX, MUX0);
	ADCSRA = setBit(ADCSRA, ADSC); // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}

uint8_t readADC2() {
	// ADMUX[1:0] = 10 (ADC2)
	// Set the active adc
	ADMUX = setBit(ADMUX, MUX1);
	ADMUX = clearBit(ADMUX, MUX0);
	ADCSRA = setBit(ADCSRA, ADSC); // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}

uint8_t readADC3() {
	// ADMUX[1:0] = 11 (ADC3)
	// Set the active adc\
	ADMUX = setBit(ADMUX, MUX1);
	ADMUX = setBit(ADMUX, MUX0);
	ADCSRA = setBit(ADCSRA, ADSC); // Store analog reading
	_delay_us(READ_DELAY_US);
	return ADCH;
}