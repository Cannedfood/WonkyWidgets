// Copyright 2018 Benno Straub, Licensed under the MIT License, a copy can be found at the bottom of this file.

#pragma once

namespace music {

enum NoteValue {
	C = 0,
	Cs = 1,
	D = 2,   Db = Cs,
	Ds = 3,
	E = 4,   Eb = Ds,
	F = 5,   Fb = E,
	Fs = 6,  Gb = Fs,
	G = 7,
	Gs = 8,  Ab = Gs,
	A = 9,
	As = 10, Bb = As,
	B = 11,
};

enum WholeNote {
	NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NOTE_A, NOTE_B
};

enum Interval {
	prime   = 0,
	min2nd  = 1,
	maj2nd  = 2,
	min3rd  = 3,
	maj3rd  = 4,
	perf4th = 5,
	aug4th  = 6,
	dim5th  = 6,
	perf5th = 7,
	aug5th  = 8,
	min6th  = 8,
	maj6th  = 9,
	min7th  = 10,
	maj7th  = 11,
	octave  = 12,
	min9th  = 13,
	maj9th  = 14,
	min10th = 15,
	maj10th = 16,
	perf11th = 17,
	aug11th  = 18
};

enum Accidental {
	FLAT,
	SHARP,
	NATURAL
};

constexpr inline
const char* to_string(NoteValue value, Accidental hint = NATURAL) noexcept {
	if(hint == NATURAL) hint = SHARP;
	switch(value) {
		case C:  return "C";
		case Cs: return "C#";
		case D:  return "D";
		case Ds: return "D#";
		case E:  return "E";
		case F:  return "F";
		case Fs: return "F#";
		case G:  return "G";
		case Gs: return "G#";
		case A:  return "A";
		case As: return "A#";
		case B:  return "B";
	}
}

constexpr inline
const char* to_string(Interval ivl, Accidental hint = NATURAL) noexcept {
	if(hint == NATURAL) hint = SHARP;
	switch(ivl) {
		case prime:    return "1st";
		case min2nd:   return "m2nd";
		case maj2nd:   return "2nd";
		case min3rd:   return "m3rd";
		case maj3rd:   return "3rd";
		case perf4th:  return "4th";
		case aug4th:   return "aug. 4th";
		case perf5th:  return "5th";
		case aug5th:   return "aug. 5th";
		case maj6th:   return "6th";
		case min7th:   return "m7th";
		case maj7th:   return "7th";
		case octave:   return "octave";
		case min9th:   return "m9th";
		case maj9th:   return "9th";
		case min10th:  return "m10th";
		case maj10th:  return "10th";
		case perf11th: return "11th";
		case aug11th:  return "aug. 11th";
		default: return "?";
	}
}

} // namespace music



// The MIT License (MIT)

// Copyright (c) 2018 Benno Straub

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.