
#include <avr/io.h>

// Frequencies for one octave of notes
#define A 440
#define As 466
#define B 494
#define C 523
#define Cs 554
#define D 587
#define Ds 622
#define E 660
#define F 698
#define Fs 740
#define G 784
#define Gs 831

// Put all frequencies in order.
const uint16_t noteToFreq[12] = {A, As, B, C, Cs, D, Ds, E, F, Fs, G, Gs};

// Define indexes of the array as scale steps
#define TONIC 0
#define MIN_SECOND 1
#define MAJ_SECOND 2
#define MIN_THIRD 3
#define MAJ_THIRD 4
#define FOURTH 5
#define DIM_FIFTH 6
#define FIFTH 7
#define MIN_SIXTH 8
#define MAJ_SIXTH 9
#define MIN_SEVENTH 10
#define MAJ_SEVENTH 11

// Create chords with scale steps
const uint8_t chordsNotes[7][4] = {
	{TONIC, MAJ_THIRD, FIFTH, MAJ_SEVENTH},
	{MAJ_SECOND, FOURTH, MAJ_SIXTH, TONIC},
	{MAJ_THIRD, FIFTH, MAJ_SEVENTH, MAJ_SECOND},
	{FOURTH, MAJ_SIXTH, TONIC, MAJ_THIRD},
	{FIFTH, MAJ_SEVENTH, MAJ_SECOND, FOURTH},
	{MAJ_SIXTH, TONIC, MAJ_THIRD, FIFTH},
	{MAJ_SEVENTH, MAJ_SECOND, FOURTH, MAJ_SIXTH},
};
