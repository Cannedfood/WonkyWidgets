// Copyright 2018 Benno Straub, Licensed under the MIT License, a copy can be found at the bottom of this file.

#include "ChordDatabase.hpp"

namespace music {

ChordDatabase::ChordDatabase() :
	mNullEntry{"Unknown", "?"}
{
	// Intervals
	registerChord("prime", "", {ABSOLUTE, {}});
	registerChord("minor 2nd", "m2nd",      {ABSOLUTE, {min2nd}});
	registerChord("major 2nd", "2nd",       {ABSOLUTE, {maj2nd}});
	registerChord("minor 3rd", "m3rd",      {ABSOLUTE, {min3rd}});
	registerChord("major 3rd", "3rd",       {ABSOLUTE, {maj3rd}});
	registerChord("4th", "4th",             {ABSOLUTE, {perf4th}});
	registerChord("diminished 5th", "dim5th", {ABSOLUTE, {dim5th}});
	registerChord("5th", "5th",             {ABSOLUTE, {perf5th}});
	registerChord("minor 6th", "m6th",      {ABSOLUTE, {min6th}});
	registerChord("major 6th", "6th",       {ABSOLUTE, {maj6th}});
	registerChord("minor 7th", "m7th",      {ABSOLUTE, {min7th}});
	registerChord("major 7th", "7th",       {ABSOLUTE, {maj7th}});

	// "Normal" triads
	registerChord("major triad",      "maj",  {ABSOLUTE, {maj3rd, perf5th}});
	registerChord("minor triad",      "m",    {ABSOLUTE, {min3rd, perf5th}});

	// "Normal" 7ths
	registerChord("major 7th",     "maj7", {ABSOLUTE, {maj3rd, perf5th, maj7th}});
	registerChord("minor 7th",     "m7",   {ABSOLUTE, {min3rd, perf5th, min7th}});
	registerChord("minor major 7th",  "m/maj7", {ABSOLUTE, {maj3rd, perf5th, min7th}});
	registerChord("dominant 7th",  "dom7", {ABSOLUTE, {maj3rd, perf5th, min7th}});

	// Aug
	registerChord("augmented triad", "aug",  {ABSOLUTE, {maj3rd, aug5th}});
	registerChord("augmented 7th",   "aug7", {ABSOLUTE, {maj3rd, aug5th, maj7th}});

	// Dim/Flat 5
	registerChord("diminished triad",    "dim",    {RELATIVE, {min3rd, min3rd}});
	registerChord("minor 7th flat 5",    "m7b5",   {ABSOLUTE, {min3rd, dim5th, min7th}});
	registerChord("dominant 7th flat 5", "dom7b5", {ABSOLUTE, {maj3rd, dim5th, min7th}});

	// Sus/Add
	registerChord("suspended 2nd",    "sus2", {ABSOLUTE, {maj2nd, perf5th}});
	registerChord("suspended 4th",    "sus4", {ABSOLUTE, {perf4th, perf5th}});
	registerChord("added 2nd",        "add2", {ABSOLUTE, {maj2nd, maj3rd, perf5th}});
	registerChord("added 4th",        "add4", {ABSOLUTE, {maj3rd, perf4th, perf5th}});
}

void ChordDatabase::generateInversions(Chord chord) {
	auto& entry = mEntries[chord];
	unsigned notes[12];
	unsigned const count = chord.apply(notes);

	unsigned invNotes[12];
	for(unsigned i = 1; i < count; i++) {
		for(unsigned j = 0; j < count; ++j)
			invNotes[j] = notes[j] + 12 - notes[i];
		Chord invertedChord(ABSOLUTE, invNotes, invNotes + count);
		auto& inversionEntry = mEntries[invertedChord];
		if(!inversionEntry) {
			inversionEntry = {
				entry.name,
				entry.shortName,
				(entry.root + count - i) % count, // Root
				i // Inversion
			};
		}
	}
}

void ChordDatabase::registerChord(const char* name, const char* shortName, Chord chord) {
	// TODO: calculate hints
	mEntries[chord] = {name, shortName};
	generateInversions(chord);
}


ChordDatabase::Entry const& ChordDatabase::get(Chord const& chord) const noexcept {
		auto iter = mEntries.find(chord);
		if(iter == mEntries.end()) return mNullEntry;
		else return iter->second;
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