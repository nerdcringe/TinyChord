#include <avr/io.h>

uint8_t setBit(uint8_t bitString, uint8_t bitToSet);
uint8_t clearBit(uint8_t bitString, uint8_t bitToClear);
uint8_t changeBit(uint8_t bitString, uint8_t bitToChange, uint8_t setting);
uint8_t checkBit(uint8_t bitString, uint8_t bitToCheck);