// Copyright 2018 Benno Straub, Licensed under the MIT License, a copy can be found at the bottom of this file.

#pragma once

#include "Chord.hpp"

#include <unordered_map>

namespace music {

class ChordDatabase {
	struct Entry {
		const char* name;
		const char* shortName;
		unsigned    root;
		unsigned    inversion;

		Entry(const char* name, const char* shortName, unsigned root = 0, unsigned inversion = 0) :
			name(name),
			shortName(shortName),
			root(root),
			inversion(inversion)
		{}

		Entry() :
			name(nullptr)
		{}

		operator bool() const noexcept {
			return name;
		}
	};
	std::unordered_map<Chord, Entry> mEntries;
	Entry                            mNullEntry;
public:
	ChordDatabase();
	void registerChord(const char* name, const char* shortName, Chord chord);
	void generateInversions(Chord chord);

	Entry const& get(Chord const& chord) const noexcept;
};

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