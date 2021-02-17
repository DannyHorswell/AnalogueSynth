#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <vector>

using namespace std;

#include "constants.h"
#include "midi.h"
#include "patch.h"
#include "filter.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "voice.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"
#include "wav.h"
#include "socket.h"
#include "benchmark.h"

using namespace std;


const int CONSOLE_BUFFER_SIZE = 128;

// Find device ids by cat /proc/asound/cards
static char device[] = "plughw:0,0";                     /* playback device */
static char mididevice[] = "hw:1,0";                     /* midi device */

static snd_pcm_format_t format = SND_PCM_FORMAT_S16;    /* sample format */

//static unsigned int buffer_time = 100000;               /* ring buffer length in us */
//static unsigned int period_time = 20000;               /* period time in us */
static unsigned int buffer_time = 20000;               /* ring buffer length in us */
static unsigned int period_time = 4000;               /* period time in us */

static int resample = 0;                                /* enable alsa-lib resampling */
static int period_event = 0;                            /* produce poll event after each period */

static snd_pcm_sframes_t buffer_size;
static snd_pcm_sframes_t period_size;
static snd_output_t *output = NULL;

#ifdef ENABLE_MIDI
	static midi theMidi;
#endif

#ifdef ENABLE_SOCKET
	static synth_socket theSocket;
#endif

snd_async_handler_t* pcm_callback;

float deltaT;

synth theSynth;

int bufferpos = 0;

 struct async_private_data {
         signed short *samples;
         snd_pcm_channel_area_t *areas;
};

void PCMCallback(snd_async_handler_t* pcm_callback)
{
#ifdef PRINT_GETNEXT
	fprintf(stderr, "PCMCallback\n");
#endif

#ifdef ENABLE_MIDI
	theMidi.pollmidi();
#endif

	snd_pcm_t* pcm_handle = snd_async_handler_get_pcm(pcm_callback);
	struct async_private_data* data = (async_private_data*) snd_async_handler_get_callback_private(pcm_callback);
	signed short* samples = data->samples;
	//snd_pcm_channel_area_t* areas = data->areas;
	snd_pcm_sframes_t avail;
	int err;

	avail = snd_pcm_avail_update(pcm_handle);

	if (avail < 0)
	{
		//fprintf(stderr, "avail negative %i\n", avail);
	}

	while (avail >= period_size)
	{
		// Fill the buffer with more data

		int offset = 0;

		for (int count=0; count<period_size; count++)
		{
			struct stereo nextVal = theSynth.getnext(deltaT);

			int valleft = nextVal.left * FLOAT_TO_SIGNED_16_MULTIPLIER;
			int valright = nextVal.right * FLOAT_TO_SIGNED_16_MULTIPLIER;
	
			for (int channel = 0 ; channel < NUMBER_OF_OP_CHANNELS; channel++)
			{
				switch (channel)
				{
					case 0:
						samples[offset++] = valleft;
						break;

					default:
						samples[offset++] = valright;
						break;
				}
			}
		}

		err = snd_pcm_writei(pcm_handle, samples, period_size);

		if (err == -EPIPE)
		/*{
			// Underrun
			fprintf(stderr, "UR", avail);
			snd_pcm_prepare(pcm_handle);
		}*/
		{ /* under-run */
			// Underrun
			fprintf(stderr, "UR", avail);
			err = snd_pcm_prepare(pcm_handle);

			if (err < 0)
			{
				printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
			} 
			else if (err == -ESTRPIPE)
			{
				while ((err = snd_pcm_resume(pcm_handle)) == -EAGAIN)
				{
					sleep(1); /* wait until the suspend flag is released */
				}
				if (err < 0)
				{
					err = snd_pcm_prepare(pcm_handle);

					if (err < 0)
					{
						printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
					}
				}
			}
		}

		else if (err < 0)
		{
			fprintf(stderr, "error from writei %s\n", snd_strerror(err));
		}

		else if (err != (int) period_size)
		{
			fprintf(stderr, "short write, write %d frames\n", snd_strerror(err));
		}

		avail = snd_pcm_avail_update(pcm_handle);
	}

#ifdef ENABLE_SOCKET
	theSocket.polllisten();
#endif
}

static int set_hwparams(snd_pcm_t *handle,
                         snd_pcm_hw_params_t *params,
                         snd_pcm_access_t access)
 {
         unsigned int rrate;
         snd_pcm_uframes_t size;
         int err, dir;
 
         /* choose all parameters */
         err = snd_pcm_hw_params_any(handle, params);
         if (err < 0) {
                 printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
                 return err;
         }
         /* set hardware resampling */
         err = snd_pcm_hw_params_set_rate_resample(handle, params, resample);
         if (err < 0) {
                 printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
                 return err;
         }
         /* set the interleaved read/write format */
         err = snd_pcm_hw_params_set_access(handle, params, access);
         if (err < 0) {
                printf("Access type not available for playback: %s\n", snd_strerror(err));
                 return err;
         }
         /* set the sample format */
        err = snd_pcm_hw_params_set_format(handle, params, format);
         if (err < 0) {
                 printf("Sample format not available for playback: %s\n", snd_strerror(err));
                 return err;
         }
         /* set the count of channels */
         err = snd_pcm_hw_params_set_channels(handle, params, NUMBER_OF_OP_CHANNELS);
         if (err < 0) {
                printf("Channels count (%i) not available for playbacks: %s\n", NUMBER_OF_OP_CHANNELS, snd_strerror(err));
                 return err;
         }
         /* set the stream rate */
         rrate = rate;
         err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
         if (err < 0) {
                 printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
                 return err;
         }
         if (rrate != rate) {
                 printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
                 //return -EINVAL;
         }
         /* set the buffer time */

         err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, &dir);

         if (err < 0) {
                 printf("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
                 return err;
         }

         err = snd_pcm_hw_params_get_buffer_size(params, &size);
         if (err < 0) {
                 printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
                 return err;
         }
         buffer_size = size;


         /* set the period time */

         err = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time, &dir);
         if (err < 0) {
                printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
                 return err;
         }
         err = snd_pcm_hw_params_get_period_size(params, &size, &dir);
         if (err < 0) {
                 printf("Unable to get period size for playback: %s\n", snd_strerror(err));
                 return err;
         }
         period_size = size;
	

         /* write the parameters to device */
         err = snd_pcm_hw_params(handle, params);
         if (err < 0) {
                 printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
                 return err;
         }
         return 0;
 }

static int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams)
 {
         int err;
 
         /* get the current swparams */
         err = snd_pcm_sw_params_current(handle, swparams);
         if (err < 0) {
                 printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
                 return err;
         }
	
         /* start the transfer when the buffer is almost full: */
         /* (buffer_size / avail_min) * avail_min */
         err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
         if (err < 0) {
                 printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
                 return err;
         }
         /* allow the transfer when at least period_size samples can be processed */
         /* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
         err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
         if (err < 0) {
                 printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
                 return err;
         }
         /* enable period events when requested */
         if (period_event) {
                err = snd_pcm_sw_params_set_period_event(handle, swparams, 1);
                 if (err < 0) {
                         printf("Unable to set period event: %s\n", snd_strerror(err));
                         return err;
                 }
         }
        /* write the parameters to the playback device */
         err = snd_pcm_sw_params(handle, swparams);
         if (err < 0) {
                 printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
                 return err;
         }

	
         return 0;
}


void MIDINoteOn(int channel, int key, int velocity)
{
	if (velocity == 0)
	{
		theSynth.keyReleased(key);
	}
	else
	{
		//fprintf(stderr, "Voice set key %i, vel %i\n", key, velocity);
		theSynth.keyPressed(key, velocity);
	}
}

void MIDINoteOff(int channel, int key, int velocity)
{
	theSynth.keyReleased(key);
}

void MIDIPitchBend(int channel, unsigned int Value)
{
	// Convert to -1.0 to 1.0 range
	// the pitch bend I tested on had a mid point of 16384
	// a a max left point of 0 and  max right point close to 65536
	// so had larger upper bent to lower bend by alot
	//
	// This gaves a range of -1.0 to _ 3.0

	float val = Value;

	float midPoint = (float) 16384;
	val = val - midPoint;
	val = val / midPoint;

	theSynth._pitchBendSemitones = val;
}

void MIDIAfterTouch(int channel, int key, int pressure)
{

}

void MIDIProgramChange(int channel, int program)
{
	fprintf(stderr, "prog chnage %i\n", program);
	theSynth.setPatch(program);
}

void SocketCommand(const string& com)
{
	int keyNo;

	//printf("%s\n",com.c_str()); // Print the command(s)

	// Single characters are easy play notes from keyboard
	if (com.size() == 1)
	{
		switch(com[0])
		{
			case 'a':
				keyNo = 60;
				break;

			case 's':
				keyNo = 62;
				break;

			case 'd':
				keyNo = 64;
				break;

			case 'f':
				keyNo = 65;
				break;

			case 'g':
				keyNo = 67;
				break;

			case 'h':
				keyNo = 69;
				break;

			case 'j':
				keyNo = 71;
				break;

			case 'k':
				keyNo = 72;
				break;

			case 'l':
				keyNo = 0;
				break;

		}

		// press the new one
		if (keyNo > -1)
		{
			theSynth.keyPressed(keyNo, 64);
		}
	}

	if (com.size() > 0)
	{
		// Split at : to get command name
		string findchar = ":";

		vector<string> splits = split(com, findchar);

		if (splits.size() == 2)
		{
			bool commandFound = false;

			if (splits[0] == "Patch")
			{
				commandFound = true;

				bool wasSampleSetChange = true;
				ProcessPatchLine(theSynth._pSelectedPatch, splits[1], wasSampleSetChange);

				if (wasSampleSetChange)
				{
					theSynth.reconfigureSampleSet();
				}
			}

			if (splits[0] == "KeyPressed")
			{
				commandFound = true;
				int key = atoi(splits[1].c_str());
				theSynth.keyPressed(key, 64);
			}

			if (splits[0] == "KeyReleased")
			{
				commandFound = true;
				int key = atoi(splits[1].c_str());
				theSynth.keyReleased(key);
			}

			if (!commandFound)
			{
				fprintf(stderr, "unknown command %s\n", splits[0].c_str());
			}
		}
	}
}

static void async_loop(snd_pcm_t *handle, signed short* samples)
 {
	printf("async_loop\n");

	struct async_private_data data;
	data.samples = samples;

    snd_async_handler_t *ahandler;
    int err, count;
 
    err = snd_async_add_pcm_handler(&ahandler, handle, PCMCallback, &data);

    if (err < 0) {
            printf("Unable to register async handler\n");
            exit(EXIT_FAILURE);
    }

	// Fill the buffer with initial data

#ifdef INITIAL_TEST_SOUND
	float testlevel = 0.0F;
#endif

	for (int per = 0; per < 3; per++) // Add 2 periods worth
	{
		int offset = 0;

		for (int count=0; count<period_size; count++)
		{

#ifdef INITIAL_TEST_SOUND
			int valleft = testlevel * FLOAT_TO_SIGNED_16_MULTIPLIER;
			int valright = testlevel * FLOAT_TO_SIGNED_16_MULTIPLIER;

			testlevel += 0.025F;

			if (testlevel > 1.0F)
			{
				testlevel = -1.0F;
			}
#else
			struct stereo nextVal = theSynth.getnext(deltaT);
			
			int valleft = nextVal.left * FLOAT_TO_SIGNED_16_MULTIPLIER;
			int valright = nextVal.right * FLOAT_TO_SIGNED_16_MULTIPLIER;
#endif

			for (int channel = 0; channel < NUMBER_OF_OP_CHANNELS; channel++)
			{
				switch (channel)
				{
					case 0:
						samples[offset++] = valleft;
						break;

					default:
						samples[offset++] = valright;
						break;
				}
			}

			//offset++;
		}

		err = snd_pcm_writei(handle, samples, period_size);

		if (err == -EPIPE)
		{
			// Underrun
			fprintf(stderr, "initial underrun occured\n");
			snd_pcm_prepare(handle);
		}
	
		else if (err < 0)
		{
			fprintf(stderr, "error from writei %s\n", snd_strerror(err));
		}
	
		else if (err != (int) period_size)
		{
			fprintf(stderr, "short write, write %d frames\n", snd_strerror(err));
		}
	
	    if (snd_pcm_state(handle) == SND_PCM_STATE_PREPARED)
		{
	            err = snd_pcm_start(handle);

	            if (err < 0)
				{
	                    printf("Start error: %s\n", snd_strerror(err));
	                    exit(EXIT_FAILURE);
	            }
	    }
	}
	
	//char theKey;
 

#ifdef ENABLE_MIDI
	//pthread_t midiThread;
	// start the MIDI reading thread

	// Set up callbacks first
	theMidi.noteOnCallback = &MIDINoteOn;
	theMidi.noteOffCallback = &MIDINoteOff;
	theMidi.pitchBendCallback =	&MIDIPitchBend;
	theMidi.afterTouchCallback = &MIDIAfterTouch;
	theMidi.programChangeCallback = &MIDIProgramChange;

	theMidi.startmidi(mididevice);
#endif

#ifdef ENABLE_SOCKET
	theSocket.tcpCommandCallback = &SocketCommand;
	theSocket.opensocket();
#endif

	char consoleLine[CONSOLE_BUFFER_SIZE];
	string com;

	// wait for input loop
	do
	{
		fgets(consoleLine, CONSOLE_BUFFER_SIZE, stdin);

		// remove the end of line character
		for (int count=0; count<CONSOLE_BUFFER_SIZE; count++)
		{
			if (consoleLine[count] == '\n')
			{
				consoleLine[count] = 0;
				break;
			}
		}

		com = consoleLine;

		printf("key %s", consoleLine);

		SocketCommand(com);
	}
	while (com != "exit");

#ifdef ENABLE_SOCKET
	theSocket.closesocket();
#endif

#ifdef ENABLE_MIDI
	theMidi.stopmidi(); // tell midi thread to terminate
	//pthread_join(midiThread, NULL);
#endif

 }
 

int main(int argc,char** argv)
{
	printf("main\n");

	srand((unsigned) time(0));

	/*benchmark theBenchmark;
	theBenchmark.run();

	return 0;*/

	snd_pcm_t* handle;
	int err;
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;

	signed short *samples;

	//printf("ALSA library version: %s\n", SND_LIB_VERSION_STR);

	// Allocate hardware parameters object
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	err = snd_output_stdio_attach(&output, stdout, 0);

    if (err < 0)
	{
        printf("Output failed: %s\n", snd_strerror(err));
        return 0;
    }
 
    printf("Playback device is %s\n", device);

	if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

	printf("snd_pcm_open\n");

	if ((err = set_hwparams(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
        printf("Setting of hwparams failed: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

	printf("set_hwparams\n");

    if ((err = set_swparams(handle, swparams)) < 0)
	{
        printf("Setting of swparams failed: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

	printf("set_swparams\n");

	// Set up buffer
	samples = (short*) malloc((period_size * NUMBER_OF_OP_CHANNELS * snd_pcm_format_physical_width(format)) / 8);

	if (!samples)
	{
        printf("No enough memory\n");
        exit(EXIT_FAILURE);
    }

	printf("malloc\n");

	// Calculate time between samples
	deltaT = 1.0F / ((float) rate);
	printf("playback interval calculated to be %f\n", deltaT);

	async_loop(handle, samples);

	// end cleanup
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(samples);
}
