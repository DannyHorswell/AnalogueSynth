#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>

#include "constants.h"
#include "voice.h"
#include "patch.h"
#include "LCRFilter.h"
#include "pan.h"
#include "wavegenerator.h"
#include "tdl.h"

#include "lfo.h"
#include "reverb.h"
#include "synth.h"

using namespace std;

extern float GDeltaT;

signal sig_dividerPerVoice = DIVEDER_PER_VOICE;

synth::synth()
{
	fprintf(stderr, "synth ctor\n");

	setMasterTune(INITIAL_MASTER_TUNE);
	
	// Initalise the voices
	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		_voices[count].init(GDeltaT);
	}

	// Initalise the LFOs
	for (int count=0; count<NUMBER_OF_LFO_FOR_SYNTH; count++)
	{
		_LFOs[count].init(count);
	}

#ifdef ENABLE_REVERB
	// Initalise Reverb
	theReverb.init();
#endif

	setPatch(0);
}

synth::~synth()
{
	fprintf(stderr, "synth dtor\n");
}

void synth::setMasterTune(float freq)
{
	_masterTune = freq;

	_keyFreqMultiplier = _masterTune / pow(2.0F, (float) MIDDLE_A_KEY_NUMBER / 12.0F);

}
	




void synth::keyPressed(const int midiKey, const int midiVelocity)
{
	signal sig_midikey(midiKey);


	printf("pathc %f\n", GPatch.WGs[0].keyOffestSemitones.ToFloat());

	float freq = GSynth.KeyNumberToFrequency(sig_midikey);

	float period = GSynth.FrequencyToPeriod(freq);

	printf("Key: %d, Velocity: %d, Frequency: %f, Period: %f\n", midiKey, midiVelocity, freq, period);


	bool found = false;

	// first see if this key is already assigned to a voice, if it is retrigger it
	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		if (_voices[count]._keyPressedInfo.key == midiKey)
		{
			found = true;
			_voices[count].setkey(midiKey, midiVelocity);
		}
	}

	if (!found)
	{
		// Round robin next voice
		_voices[nextVoice++].setkey(midiKey, midiVelocity);
	}

	if (nextVoice >= NUMBER_OF_VOICES)
	{
		printf("Sero\n");
		nextVoice = 0;
	}
}


void synth::keyReleased(int midiKey)
{
	printf("Key released %d, %d\n", midiKey);

	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		if (_voices[count]._keyPressedInfo.key == midiKey)
		{
			_voices[count].releasekey();
		}
	}
}

void synth::setPatch(int patchNumber)
{
	
}

void synth::InitalisePatches()
{
	fprintf(stderr, "Loading patches\n");

	PopulateDefaultPatch();

	fprintf(stderr, "Patches loaded\n");
}



