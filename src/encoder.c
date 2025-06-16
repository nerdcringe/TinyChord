#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t ENC_A = 0;
uint8_t ENC_B = 0;

volatile int8_t lastA = 0;
volatile int8_t lastB = 0;


volatile int8_t encoderValue = 122;

#define CHECK_BIT(var,pos) (((var) & (1<<(pos))) > 0)

void initEncoder(uint8_t ENC_A_, uint8_t ENC_B_) {
	ENC_A = ENC_A_;
	ENC_B = ENC_B_;
	// Input pullups
	PORTB |= (1<<ENC_A);
	PORTB |= (1<<ENC_B);

	// Pin-change interrupts
	PCMSK |= (1<<ENC_A) | (1<<ENC_B); // Set pins to monitor
	GIMSK |= 1<<PCIE; // Enable pin-change interrupt
	GIFR |= 1<<PCIF; // Clear interrupt flag
	sei(); // Set enable interrupts
}

int8_t getEncoderValue() {
	return encoderValue;
}

void setEncoderValue(int8_t newValue) {
	encoderValue = newValue;
}


ISR (PCINT0_vect) {
	uint8_t A = CHECK_BIT(PINB, ENC_A);
	uint8_t B = CHECK_BIT(PINB, ENC_B);

	
	if (A != lastA) {
		lastA = A;
		if (B != lastB) {
			lastB = B;
		}
		if (A == B) {
			encoderValue++;
		} else {
			encoderValue--;
		}
	}
}