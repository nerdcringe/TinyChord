#include <avr/io.h>

void initEncoder(uint8_t CLK_, uint8_t DT_);
int8_t getEncoderValue();
void setEncoderValue(int8_t newValue);