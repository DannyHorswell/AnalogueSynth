
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ENABLE_TDP	// Enable time dependant pitch
#define ENABLE_TDA	// Enable time dependant amplitude
#define ENABLE_TDF	// Enable time dependant filter

#define ENABLE_FILTERS
//#define ENABLE_OUTPUT_COMPRESSION
#define ENABLE_REVERB
#define NORMALSE_SAMPLES

#ifdef __arm__
#define ENABLE_MIDI
#endif

//#define PRINT_GETNEXT // Used for debuging generation loop issues, under runs will occur, but can tell how far it gets if getting seg faults


#define INITIAL_TEST_SOUND // if defined, the sound buffers are initially filled with a test sound to test the sound system

// Options
#ifdef _WIN32
#define SAMPLE_FREQUENCY (44100)
#endif

#ifdef __arm__
#define SAMPLE_FREQUENCY (48000)
#endif

#define FRAMES_PER_BUFFER  (64)

#define NUMBER_OF_OP_CHANNELS 2						/* number of output channels 1 or 2 */
#define NUMBER_OF_WAVE_GENERATORS_PER_VOICE 6
#define NUMBER_OF_LFO_FOR_SYNTH 8
#define NUMBER_OF_VOICES 12
#define DIVEDER_PER_VOICE 3.5 // SQRT OF ABOVE

#define MAX_SAMPLES_PER_SAMPLESET 16

#define INITIAL_MASTER_TUNE  440.0F

#define MAX_REVERB_SAMPLES 20000

// Main thread poll time
#define MAIN_THREAD_SLEEP_PERIOD_MS 2

// TCP
#define TCP_PORT 1234

#define MAX_VAL (32767)
#define MIN_VAL (-32767)

// Mathmatical
#define PI 3.1415926535F
#define TWO_PI 6.2831853072F
#define HALF_PI 1.57079632675F
#define QUATER_PI 0.785398163375F
#define PI_SQUARED 9.869604401089F

// MIDI
#define MIDDLE_A_KEY_NUMBER  69

// Scales
#define FLOAT_TO_SIGNED_16_MULTIPLIER (32767)
#define FLOAT_TO_SIGNED_24_MULTIPLIER 8388352.0F

#define CONTROL_7F_TO_FLOAT_0_1 (0.0078125F)

#define WAVE_FORMAT_IEEE_FLOAT 3

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
class LCRFilter;
class noise;
class tdl;

#define reset_consolecolour		printf("\033[1;0m");
#define red_consolecolour		printf("\033[1;31m");
#define green_consolecolour		printf("\033[1;32m");
#define yellow_consolecolour	printf("\033[1;33m");
#define blue_consolecolour		printf("\033[1;34m");
#define purple_consolecolour	printf("\033[1;35m");
#define cyan_consolecolour		printf("\033[1;36m");
#define white_consolecolour		printf("\033[1;37m");



#define B_ 1.2732395447F //B = 4.0 / PI;
#define C_ -0.405284735F //C = -4.0 /(PI_SQUARED);

#define VOLUME_MAPPING_STEPS 4096


// Fast approximation for sin
inline float wg_sin(float x)
	{
		if (x > PI)
		{
			x -= TWO_PI;
		}

		return -(B_ * x + C_ * x * ((x < 0) ? -x : x));
	}

#endif



