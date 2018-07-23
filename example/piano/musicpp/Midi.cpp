#include "Midi.hpp"

#include <iostream>
#include <memory.h>

// TODO

namespace music::midi {

namespace {

struct Chunk {
	char     chunk_name[4];
	uint32_t chunk_length;
};

struct HeaderChunkData {
	uint16_t format;
	uint16_t tracks;
	uint16_t division;

	void read(std::istream& s) {
		s.read((char*)this, sizeof(HeaderChunkData));
	}
};

}

Event parseEvent(uint8_t** pdata) {
	uint8_t* data = *pdata;

	*pdata = data;

	Event e;
	return e;
}

void File::parse(std::istream& stream) {
	while(!stream.eof()) {
		Chunk chunk;
		stream.read((char*)&chunk, sizeof(Chunk));

		if(!memcmp(chunk.chunk_name, "MThd", 4)) {
			HeaderChunkData header;
			stream.read((char*)&header, sizeof(Chunk));
			tracks.reserve(header.tracks);

			continue;
		}

		if(!memcmp(chunk.chunk_name, "MTrk", 4)) {
			std::vector<uint8_t> data(chunk.chunk_length);
			stream.read((char*)data.data(), data.size());

			Track track;

			uint8_t* pdata = data.data();
			while(pdata < (data.data() + data.size())) {
				track.events.push_back(parseEvent(&pdata));
			}

			tracks.push_back(track);

			continue;
		}

		stream.seekg(chunk.chunk_length, std::ios::cur);
	}
}

File::File(std::istream& stream) {
	parse(stream);
}

} // namespace music::midi