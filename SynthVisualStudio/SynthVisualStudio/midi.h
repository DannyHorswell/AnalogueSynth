#ifndef MIDI_H
#define MIDI_H


#ifdef ENABLE_MIDI
typedef void (*NoteOff)(int channel, int key, int velocity);
typedef void (*NoteOn)(int channel, int key, int velocity); 
typedef void (*AfterTouch)(int channel, int key, int pressure);
// TODO Control mode
typedef void (*ProgramChange)(int channel, int program);

typedef void (*PitchBend)(int channel, unsigned int amount); 
typedef void (*ControlChange)(int channel, unsigned int controlNumber, unsigned int value); 

const int MIDI_DATA_BUFFER_SIZE = 256;

enum class messageType
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

enum class controlChangeType
{
	BANK_SELECT				= 0x00,
	MODULATION				= 0x01,
	BREATH_MSB				= 0x02,
	FOOT_MSB				= 0x04,
	PORTAMENTO_TIME_MSB		= 0x05,
	DATA_ENTRY_MSB			= 0x06,
	MAIN_VOLUME_MSB    		= 0x07,
	BALANCE_MSB        		= 0x08,
	PAN_MSB            		= 0x0a,
	EXPRESSION_MSB       	= 0x0b,
	EFFECT1_MSB				= 0x0c,
	EFFECT2_MSB				= 0x0d,
	GENERAL_PURPOSE1_MSB 	= 0x10,
	GENERAL_PURPOSE2_MSB 	= 0x11,
	GENERAL_PURPOSE3_MSB 	= 0x12,
	GENERAL_PURPOSE4_MSB 	= 0x13,
	BANK					= 0x20,
	MODWHEEL        		= 0x21,
	BREATH           		= 0x22,
	FOOT             		= 0x24,
	PORTAMENTO_TIME 		= 0x25,
	DATA_ENTRY				= 0x26,
	MAIN_VOLUME      		= 0x27,	
	BALANCE          		= 0x28,
	PAN              		= 0x2a,
	EXPRESSION       		= 0x2b,
	EFFECT1					= 0x2c,
	EFFECT2					= 0x2d,
	GENERAL_PURPOSE1 		= 0x30,
	GENERAL_PURPOSE2 		= 0x31,
	GENERAL_PURPOSE3 		= 0x32,
	GENERAL_PURPOSE4 		= 0x33,
	SUSTAIN              	= 0x40,
	PORTAMENTO           	= 0x41,
	SOSTENUTO            	= 0x42,
	SUSTENUTO            	= 0x42,
	SOFT_PEDAL           	= 0x43,
	LEGATO_FOOTSWITCH		= 0x44,
	HOLD2                	= 0x45,
	SC1_SOUND_VARIATION		= 0x46,
	SC2_TIMBRE				= 0x47,
	SC3_RELEASE_TIME		= 0x48,
	SC4_ATTACK_TIME			= 0x49,
	SC5_BRIGHTNESS			= 0x4a,
	SC6						= 0x4b,
	SC7						= 0x4c,
	SC8						= 0x4d,
	SC9						= 0x4e,
	SC10					= 0x4f,
	GENERAL_PURPOSE5     	= 0x50,
	GENERAL_PURPOSE6     	= 0x51,
	GENERAL_PURPOSE7     	= 0x52,
	GENERAL_PURPOSE8     	= 0x53,
	PORTAMENTO_CONTROL		= 0x54,
	E1_REVERB_DEPTH			= 0x5b,
	E2_TREMOLO_DEPTH		= 0x5c,
	E3_CHORUS_DEPTH			= 0x5d,
	E4_DETUNE_DEPTH			= 0x5e,
	E5_PHASER_DEPTH			= 0x5f,
	DATA_INCREMENT       	= 0x60,
	DATA_DECREMENT       	= 0x61,
	NONREG_PARM_NUM_LSB  	= 0x62,
	NONREG_PARM_NUM_MSB  	= 0x63,
	REGIST_PARM_NUM_LSB  	= 0x64,
	REGIST_PARM_NUM_MSB		= 0x65,
	ALL_SOUNDS_OFF			= 0x78,
	RESET_CONTROLLERS		= 0x79,
	LOCAL_CONTROL_SWITCH	= 0x7a,
	ALL_NOTES_OFF			= 0x7b,
	OMNI_OFF				= 0x7c,	
	OMNI_ON					= 0x7d,
	MONO1					= 0x7e,
	MONO2					= 0x7f,
};

class midi
{
	snd_rawmidi_t *handle_in;

	unsigned char ch;

	messageType lastMessageType;
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
	ControlChange controlChangeCallback;
};
#endif
#endif


