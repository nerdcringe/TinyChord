
#include <avr/io.h>

// Frequencies for one octave of notes
const uint16_t noteToFreq[12] = {440, 466, 494, 523, 554, 587, 622, 660, 698, 740, 784, 831};

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
// Provide the default (diatonic) steps for a major scale
const uint8_t defaultNotes[7][4] = {
	{TONIC, MAJ_THIRD, FIFTH, MAJ_SEVENTH},
	{MAJ_SECOND, FOURTH, MAJ_SIXTH, TONIC},
	{MAJ_THIRD, FIFTH, MAJ_SEVENTH, MAJ_SECOND},
	{FOURTH, MAJ_SIXTH, TONIC, MAJ_THIRD},
	{FIFTH, MAJ_SEVENTH, MAJ_SECOND, FOURTH},
	{MAJ_SIXTH, TONIC, MAJ_THIRD, FIFTH},
	{MAJ_SEVENTH, MAJ_SECOND, FOURTH, MAJ_SIXTH},
};

// Swap major and minor chords
// Specifically maj7 -> min7 and min7 -> 7 (dominant)
const uint8_t moodSwapNotes[7][4] = {
	{TONIC, MIN_THIRD, FIFTH, MIN_SEVENTH},
	{MAJ_SECOND, DIM_FIFTH, MAJ_SIXTH, TONIC},
	{MAJ_THIRD, MIN_SIXTH, MAJ_SEVENTH, MAJ_SECOND},
	{FOURTH, MIN_SIXTH, TONIC, MIN_THIRD},
	{FIFTH, MIN_SEVENTH, MAJ_SECOND, FOURTH},
	{MAJ_SIXTH, MIN_SECOND, MAJ_THIRD, FIFTH},
	{MAJ_SEVENTH, MAJ_SECOND, DIM_FIFTH, MAJ_SIXTH},
};

// Replace chords with fully diminished versions
// Diminished notes are 3 semitones apart
const uint8_t diminishedNotes[7][4] = {
	{MIN_SECOND, MIN_SECOND+3, MIN_SECOND+6, MIN_SECOND+9},
	{MAJ_SECOND, MAJ_SECOND+3, MAJ_SECOND+6, MAJ_SECOND+9},
	{MAJ_THIRD, MAJ_THIRD+3, MAJ_THIRD+6, MAJ_THIRD+9},
	{DIM_FIFTH, DIM_FIFTH+3, DIM_FIFTH+6, DIM_FIFTH+9},
	{MIN_SIXTH, MIN_SIXTH+3, MIN_SIXTH+6, MIN_SIXTH+9},
	{MAJ_SIXTH, MAJ_SIXTH+3, MAJ_SIXTH+6, MAJ_SIXTH+9},
	{MAJ_SEVENTH, MAJ_SEVENTH+3, MAJ_SEVENTH+6, MAJ_SEVENTH+9},
};
