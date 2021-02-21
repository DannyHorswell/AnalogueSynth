#ifndef MIDI_H
#define MIDI_H

#ifdef ENABLE_MIDI
typedef void (*NoteOff)(int channel, int key, int velocity);
typedef void (*NoteOn)(int channel, int key, int velocity); 
typedef void (*AfterTouch)(int channel, int key, int pressure);
// TODO Control mode
typedef void (*ProgramChange)(int channel, int program);

typedef void (*PitchBend)(int channel, unsigned int amount); 

const int MIDI_DATA_BUFFER_SIZE = 256;

enum control
{
	UNDEFINED				= 0x00,

	// LS 4 bits = Channel
	NOTE_OFF				= 0x80,
	NOTE_ON					= 0x90,
	POLY_AFTER_TOUCH		= 0xA0,
	CONTROL_CHANGE			= 0xB0,
	PROGRAM_CHANGE			= 0xC0,
	CHAN_AFTER_TOUCH		= 0xD0,
	PITCH_WHEEL				= 0xE0,

	SYSEXC					= 0xF0,
	MIDI_TIME_QTR_FRAME		= 0xF1,
	SONG_POS				= 0xF2,
	SONG_SELECT				= 0xF3,
	UNDEFINED1				= 0xF4,
	UNDEFINED2				= 0xF5,
	TUNE_REQUEST			= 0xF6,
	END_SYSEX				= 0xF7,
	TIMING_CLOCK			= 0xF8,
	UNDEFINED3				= 0xF9,
	START					= 0xFA,
	CONTINIE				= 0xFB,
	STOP					= 0xFC,
	UNDEFINED4				= 0xFD,
	ACTIVE_SENSING			= 0xFE,
	SYSTEM_RESET			= 0xFF,
};

class midi
{
	snd_rawmidi_t *handle_in;

	unsigned char ch;

	control lastControl;
	char channel;

	int bytecount;
	unsigned char data[MIDI_DATA_BUFFER_SIZE]; // Following databytes

public:
	void startmidi(char*);
	
	void pollmidi();
	
	void stopmidi();

	// MIDI callbacks
	NoteOff noteOffCallback;
	NoteOn noteOnCallback;
	AfterTouch afterTouchCallback;
	// TODO Control mode
	ProgramChange programChangeCallback;
	PitchBend pitchBendCallback;
};
#endif

#endif

