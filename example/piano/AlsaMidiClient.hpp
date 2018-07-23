#pragma once

extern "C" {
#include <alsa/asoundlib.h>
}

class AlsaMidiClient {
	snd_seq_t* mSequencer;

	int mInputPort;
public:
	AlsaMidiClient();
	~AlsaMidiClient();
	void pollMidiEvents();

protected:
	virtual void noteOn (unsigned note) = 0;
	virtual void noteOff(unsigned note) = 0;
};