#include <avr/io.h>

void initSynth();
void disableSynth();
void enableSynth();
void setJump(uint8_t jumpIndex, uint32_t newJump);
