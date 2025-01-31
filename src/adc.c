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
}

uint8_t readADC3() {
	ADMUX &= ~(1<<MUX0); // use ADC3
	ADCSRA = setBit(ADCSRA, ADSC);// Convert the current analog input value
	_delay_us(READ_DELAY_US);
	return ADCH;
}

uint8_t readADC2() {
	ADMUX |= (1<<MUX0); // use ADC2
	ADCSRA = setBit(ADCSRA, ADSC);// Convert the current analog input value
	_delay_us(READ_DELAY_US);
	return ADCH;
}