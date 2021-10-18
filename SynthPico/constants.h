#pragma once


//#define GENERATE_TEST_SOUND // Bypasses synth and returns a test sin wave

#define ENABLE_TDP	// Enable time dependant pitch
#define ENABLE_TDA	// Enable time dependant amplitude
//#define ENABLE_TDF	// Enable time dependant filter

//#define ENABLE_FILTERS
//#define ENABLE_REVERB

#define ENABLE_MIDI

//#define PRINT_GETNEXT // Used for debuging generation loop issues, under runs will occur, but can tell how far it gets if getting seg faults

// Options
#define SAMPLE_FREQUENCY (24000)

#define FRAMES_PER_BUFFER  (64)

#define NUMBER_OF_OP_CHANNELS (2)						/* number of output channels 1 or 2 */
#define NUMBER_OF_WAVE_GENERATORS_PER_VOICE (2)
#define DIVEDER_PER_WG (1.417F) // SQRT OF ABOVE

#define NUMBER_OF_LFO_FOR_SYNTH (4)
#define NUMBER_OF_VOICES (1)
#define DIVEDER_PER_VOICE (1.414F) // SQRT OF ABOVE

#define INITIAL_MASTER_TUNE  440.0F

#define MAX_REVERB_SAMPLES 10000

// Main thread poll time
#define MAIN_THREAD_SLEEP_PERIOD_MS 2

#define N_PATCHES 128


// MIDI
#define MIDDLE_A_KEY_NUMBER  69

struct stereo
{
	signal left;
	signal right;
};

class synth;
struct patch;
//class voice;
//class wavegenerator;
class lfo;
class reverb;
//class LCRFilter;
class noise;
class tdl;



#define VOLUME_MAPPING_STEPS 4096

extern synth GSynth; // Global single instance of the synyh
extern patch GPatch; // Gloabal single instance of current patch data



