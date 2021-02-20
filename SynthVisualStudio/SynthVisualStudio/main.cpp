#include <stdlib.h>
#include <string>
#include <stdio.h>

#ifdef __arm__
#include <pthread.h>
#endif


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
#include "portaudio.h"

using namespace std;


const int CONSOLE_BUFFER_SIZE = 128;

// Find device ids by cat /proc/asound/cards
static char device[] = "plughw:0,0";                     /* playback device */
static char mididevice[] = "hw:1,0";                     /* midi device */

//static unsigned int buffer_time = 100000;               /* ring buffer length in us */
//static unsigned int period_time = 20000;               /* period time in us */
static unsigned int buffer_time = 20000;               /* ring buffer length in us */
static unsigned int period_time = 4000;               /* period time in us */

static int resample = 0;                                /* enable alsa-lib resampling */
static int period_event = 0;                            /* produce poll event after each period */

PortAudio ThePortAudio;

#ifdef ENABLE_MIDI
	static midi theMidi;
#endif

#ifdef ENABLE_SOCKET
	static SocketClient theSocket;
#endif


float deltaT;

synth theSynth;

int bufferpos = 0;



 static PaError StreamCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
 {
#ifdef __arm__
	 /* Cast data passed through stream to our structure type. */
	 float* out = (float*)outputBuffer;
	 float value = 0.0f;
#else
	 /* Cast data passed through stream to our structure type. */
	 INT16* out = (INT16*)outputBuffer;
	 INT16 value = 0;
#endif

	 for (unsigned int i = 0; i < framesPerBuffer; i++)
	 {
		 struct stereo nextVal = theSynth.getnext(deltaT);

		 int valleft = nextVal.left * FLOAT_TO_SIGNED_16_MULTIPLIER;
		 int valright = nextVal.right * FLOAT_TO_SIGNED_16_MULTIPLIER;


#ifdef __arm__
		 * out++ = valleft / MAX_VAL;
#else
		 * out++ = valleft;
#endif 
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

static void async_loop()
 {
	printf("async_loop\n");

	ThePortAudio.Initalise(StreamCallback);
	
	

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

	async_loop();
}
