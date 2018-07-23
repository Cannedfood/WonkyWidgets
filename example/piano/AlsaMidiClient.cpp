#include "AlsaMidiClient.hpp"

#include <stdexcept>

static
void errorHandler(const char *file, int line, const char *function, int err, const char *fmt, ...) {
	va_list arg;

	if (err == ENOENT)	/* Ignore those misleading "warnings" */
		return;
	va_start(arg, fmt);
	fprintf(stderr, "ALSA lib %s:%i:(%s) ", file, line, function);
	vfprintf(stderr, fmt, arg);
	if (err)
		fprintf(stderr, ": %s", snd_strerror(err));
	putc('\n', stderr);
	va_end(arg);
}

AlsaMidiClient::AlsaMidiClient() {
	int err = snd_seq_open(&mSequencer, "default", SND_SEQ_OPEN_INPUT, SND_SEQ_NONBLOCK);
	if(err < 0) throw std::runtime_error("Failed opening alsa sequencer");
	snd_seq_set_client_name(mSequencer, "Piano booster");

	snd_lib_error_set_handler(errorHandler);

	mInputPort = snd_seq_create_simple_port(mSequencer, "input", SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_APPLICATION);

	// Auto connect

	snd_seq_connect_from(mSequencer, mInputPort, 28, 0);
}

AlsaMidiClient::~AlsaMidiClient() {
	snd_seq_close(mSequencer);
}

static inline
snd_seq_event_t* readEvent(snd_seq_t* seq) {
	snd_seq_event_t* ev = NULL;
	snd_seq_event_input(seq, &ev);
	return ev;
}

void AlsaMidiClient::pollMidiEvents() {
	while(snd_seq_event_t* ev = readEvent(mSequencer)) {
		switch(ev->type) {
			case SND_SEQ_EVENT_NOTEON:
				if(ev->data.note.velocity != 0) {
					noteOn(ev->data.note.note);
					break;
				}
				else [[fallthrough]];
			case SND_SEQ_EVENT_NOTEOFF: noteOff(ev->data.note.note); break;
			default: break;
		}
	}
}