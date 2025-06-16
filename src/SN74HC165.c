#include "SN74HC165.h"
#include "adc.h"
#include <util/delay.h>

#define CHECK_BIT(var,pos) (((var) & (1<<(pos))) > 0)

uint8_t numBits;
//uint8_t data; NOW USING ADC ON RST PIN
uint8_t clock;
uint8_t latch;

void initInputRegister(uint8_t _numBits/*, uint8_t _data*/, uint8_t _clock, uint8_t _latch) {
	numBits = _numBits;
	//data = _data;
	clock = _clock;
	latch = _latch;
	//DDRB &= ~(1<<data); // Data is input
	DDRB |= (1<<clock); // Clock is output
	DDRB |= (1<<latch); // Latch is output
}

// Read from the SN74HC165 shift register
uint16_t readInputRegister() {
	// Pulse the latch to shift in the current input values
	PORTB &= ~(1<<latch);
	_delay_us(LATCH_DELAY);
	PORTB |= (1<<latch);
 
	// Read every bit one-by-one, pulsing the clock each time to shift the next bit in
	uint16_t incoming = 0;
	for (int i = 0; i < numBits; i++) {
		// set clock to 0 to get next bit
		PORTB &= ~(1<<clock);

		// Read current bit
		//if (CHECK_BIT(PINB, data)) {

		// Use ADC on reset pin for data input (use external pullup to keep it mostly high)
		// Cannot go below 1.4V or else microcontroller will reset
		if (readADC0() > 240) {
			incoming |= (1<<i);
		}
		
		PORTB |= (1<<clock); // set clock to 1 before next read
	}
	return incoming;
}

