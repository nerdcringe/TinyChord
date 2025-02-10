#include "mymath.h"
#include <math.h>

int intPythag(int a, int b) {
	if (a < 0) {
		a = -a;
	}
	if (b < 0) {
		b = -b;
	}
	if (a < b) {
		return ((M_SQRT2-1) * a) + b;
	} else {
		return ((M_SQRT2-1) * b) + a;
	}
}

//https://docs.arduino.cc/language-reference/en/functions/math/map/
int map(int x, int in_min, int in_max, int out_min, int out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
