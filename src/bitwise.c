#include "bitwise.h"

uint8_t setBit(uint8_t bitString, uint8_t bitToSet) {
    return bitString | (1<<bitToSet);
}

uint8_t clearBit(uint8_t bitString, uint8_t bitToClear) {
    return bitString & ~(1<<bitToClear);
}

uint8_t changeBit(uint8_t bitString, uint8_t bitToChange, uint8_t setting) {
	if (setting) {
		bitString |= (1<<bitToChange);
	} else {
		bitString &= ~(1<<bitToChange);
	}
    return bitString;
}

uint8_t checkBit(uint8_t bitString, uint8_t bitToCheck) {
	return (bitString & (1<<bitToCheck)) > 0;
}