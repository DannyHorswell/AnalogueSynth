
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ENABLE_TDP	// Enable time dependant pitch
#define ENABLE_TDA	// Enable time dependant amplitude
//#define ENABLE_TDF	// Enable time dependant filter

#define ENABLE_FILTERS
//#define ENABLE_OUTPUT_COMPRESSION
#define ENABLE_REVERB
#define NORMALSE_SAMPLES
#define ENABLE_MIDI
#define ENABLE_SOCKET

//#define PRINT_GETNEXT // Used for debuging generation loop issues, under runs will occur, but can tell how far it gets if getting seg faults


#define INITIAL_TEST_SOUND // if defined, the sound buffers are initially filled with a test sound to test the sound system

// Options
static unsigned int rate	= 44100;                     /* stream rate */
//static unsigned int rate	= 22050;                       /* stream rate */
//static unsigned int rate = 11025;                       /* stream rate */

#define NUMBER_OF_OP_CHANNELS 2						/* number of output channels 1 or 2 */
#define NUMBER_OF_WAVE_GENERATORS_PER_VOICE 2
#define NUMBER_OF_LFO_FOR_SYNTH 2
#define NUMBER_OF_VOICES 12
#define MAX_SAMPLES_PER_SAMPLESET 16

#define INITIAL_MASTER_TUNE  440.0F

#define MAX_REVERB_SAMPLES 20000

// TCP
#define TCP_PORT 1234

// Mathmatical
#define PI 3.1415926535F
#define TWO_PI 6.2831853072F
#define PI_SQUARED 9.869604401089F

// MIDI
#define MIDDLE_A_KEY_NUMBER  69

// Scales
#define FLOAT_TO_SIGNED_16_MULTIPLIER 32767.0F
#define FLOAT_TO_SIGNED_24_MULTIPLIER 8388352.0F

struct stereo
{
	float left;
	float right;
};

class synth;
class voice;
class wavegenerator;
class lfo;
class reverb;
class filter;
class noise;
class tdl;

#endif



