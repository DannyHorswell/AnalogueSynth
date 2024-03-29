#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <thread>


#ifdef __arm__
#include <alsa/asoundlib.h>
#include <chrono>
#endif

uint64_t lastStart;
float percent;

// Get time stamp in microseconds.
uint64_t micros()
{
	uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::
		now().time_since_epoch()).count();
	return us;
}

using namespace std;

#include "constants.h"
#include "voice.h"
#include "midi.h"
#include "patch.h"
#include "LCRFilter.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"
#include "benchmark.h"

using namespace std;


const int CONSOLE_BUFFER_SIZE = 128;

// Find device ids by cat /proc/asound/cards
static char device[] = "plughw:0,0";                     /* playback device */

// Find midi device Id's by amidi -l
static char mididevice[] = "hw:2,0";                     /* midi device */

static int resample = 0;                                /* enable alsa-lib resampling */
static int period_event = 0;                            /* produce poll event after each period */

PortAudio ThePortAudio;

#ifdef ENABLE_MIDI
	static midi theMidi;
#endif


bool running = true;

std::thread* pConsoleLoopThread = NULL;
std::thread* pSocketReadThread = NULL;


float deltaT = 1.0F / (float) SAMPLE_FREQUENCY; // Time between samples

synth theSynth(deltaT);

int bufferpos = 0;




static int lrCount = 0;

 static PaError StreamCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
 {
	 uint64_t start = micros();

#ifdef __arm__
	 /* Cast data passed through stream to our structure type. */
	 float* out = (float*)outputBuffer;
	 float value = 0.0f;
#else
	 /* Cast data passed through stream to our structure type. */
	 INT16* out = (INT16*)outputBuffer;
	 INT16 value = 0;
#endif

	struct stereo nextVal;

	 for (unsigned int i = 0; i < framesPerBuffer; i++)
	 {
		nextVal = theSynth.getnext(deltaT);
	 

#ifdef __arm__
* out++ = nextVal.left;
		//* out++ = 0.0F;  
		* out++ = nextVal.right;
		
#else		 
		* out++ = nextVal.left;
		* out++ = nextVal.right; 
#endif 

		lrCount++;
	 }
	 

	 uint64_t end = micros();

	 percent = ((float) (end - start)) * 100.0F / ((float) (start - lastStart));

	 lastStart = start;


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

	printf("%s\n",com.c_str()); // Print the command(s)

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

			case '%':
				printf("Percent load %.2f\n", percent);
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

				bool wasSampleSetChange = false;
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

void ConsoleLoop()
{
	char consoleLine[CONSOLE_BUFFER_SIZE];

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

		printf("key %s\n", consoleLine);

		SocketCommand(consoleLine);
	}
	while (consoleLine != "exit");
	
	running = false;
}

void SocketReadingLoop()
{
	TheSocket.Open();
}

void MainProcessingLoop()
{
    printf("Entering MainProcessingLoop\n");

    while (running)
    {
#ifdef ENABLE_MIDI
		theMidi.pollmidi();
#endif

        Pa_Sleep(MAIN_THREAD_SLEEP_PERIOD_MS);
    }

    printf("Exiting MainProcessingLoop\n");
 }




int Xmain(int argc,char** argv)
{
	printf("main\n");
	printf("Delta T %f\n", deltaT);

	srand((unsigned) time(0));

	// Initalise TDL volume mappings
	InitVolumnMapping();

	//WriteTestToFile();
	//return 0;

	ThePortAudio.Initalise(StreamCallback);
	
	printf("Starting console loop\n");
	pConsoleLoopThread = new std::thread(ConsoleLoop);

	printf("Starting socket loop\n");
	pSocketReadThread = new std::thread(SocketReadingLoop);

	printf("Threads started\n");

#ifdef ENABLE_MIDI
	//pthread_t midiThread;
	// start the MIDI reading thread

	// Set up callbacks first
	theMidi.noteOnCallback = &MIDINoteOn;
	theMidi.noteOffCallback = &MIDINoteOff;	char consoleLine[CONSOLE_BUFFER_SIZE];
	string com;


	theMidi.pitchBendCallback =	&MIDIPitchBend;
	theMidi.afterTouchCallback = &MIDIAfterTouch;
	theMidi.programChangeCallback = &MIDIProgramChange;

	theMidi.startmidi(mididevice);

#endif

	MainProcessingLoop();
	
	if (pConsoleLoopThread != NULL)
    {
        pConsoleLoopThread->join();
        delete(pConsoleLoopThread);
    }
    
#ifdef ENABLE_MIDI
	theMidi.stopmidi(); // stop midi
#endif

#ifdef ENABLE_SOCKET
	theSocket.closesocket();
#endif



	
}
