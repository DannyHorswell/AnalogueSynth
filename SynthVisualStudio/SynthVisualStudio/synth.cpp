#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>

#include "constants.h"
#include "patch.h"
#include "LCRFilter.h"
#include "pan.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "voice.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"

using namespace std;

const int LFO_RECALC_NUM = 256;


synth::synth(float deltaT)
{
	fprintf(stderr, "synth ctor\n");

	setMasterTune(INITIAL_MASTER_TUNE);

	_pSelectedPatch = _allPatches;
	
	InitalisePatches();
	
	// Initalise the voices
	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		_voices[count].init(this, deltaT);
	}

	// Initalise the LFOs
	for (int count=0; count<NUMBER_OF_LFO_FOR_SYNTH; count++)
	{
		_LFOs[count].init(this, count);
	}

	// Initalise Reverb
	theReverb.init(this);

	setPatch(0);

	// load PCM Samples
	//sampleSets[0].init(8);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C1.wav"), 32.703F, 0, 0);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C2.wav"), 65.406F, 0, 0);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C3.wav"), 130.81F, 0, 0);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C4.wav"), 261.63F, 0, 0);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C5.wav"), 523.25F, 0, 0);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C6.wav"), 1046.50F, 0, 0);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C7.wav"), 2093.00F, 0, 0);
	//sampleSets[0].loadFile(string("samples/PIANO_LOUD_C8.wav"), 4186.01F, 0, 0);

	//sampleSets[0].init(4);

	//sampleSets[0].loadFile(string("samples/CELLO_C2.wav"), 65.406F, 0, 0);
	//sampleSets[0].loadFile(string("samples/CELLO_C3.wav"), 130.81F, 0, 0);
	//sampleSets[0].loadFile(string("samples/CELLO_C4.wav"), 261.63F, 0, 0);
	//sampleSets[0].loadFile(string("samples/CELLO_C5.wav"), 523.25F, 0, 0);

	/*sampleSets[0].init(4);

	sampleSets[0].loadFile(string("samples/BASOON_C2.wav"), 65.406F, 0, 0);
	sampleSets[0].loadFile(string("samples/BASOON_C3.wav"), 130.81F, 0, 0);
	sampleSets[0].loadFile(string("samples/BASOON_C4.wav"), 261.63F, 0, 0);
	sampleSets[0].loadFile(string("samples/BASOON_C5.wav"), 523.25F, 0, 0);*/

	//fprintf(stderr, "wav loaded\n");
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
	

float synth::KeyNumberToFrequency(float keyNumber)
{
	float freq = pow(2.0F, keyNumber / 12.0F) * _keyFreqMultiplier;

	return freq;
}

float synth::KeyToPeriod(float keyNumber)
{
	return (1.0F / KeyNumberToFrequency(keyNumber));
}

float compressionLevel = 1.0F;

stereo synth::getnext(float deltaT)
{
#ifdef PRINT_GETNEXT
	printf("synth::getnext\n");
#endif

	// Recalculate LFOs
	if (lfoRecalcCount++ == 0)
	{
		for (int count=0; count<NUMBER_OF_LFO_FOR_SYNTH; count++)
		{
			float val =_LFOs[count].getnext(lfoDeltaT);
		}
		
		lfoDeltaT = 0;
	}
	else
	{
		lfoDeltaT += deltaT;
	}

	if (lfoRecalcCount >= LFO_RECALC_NUM)
	{
		lfoRecalcCount = 0;
	}

	// Mixes all voices
	output.left = 0.0F;
	output.right = 0.0F;

	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		stereo voiceOp = _voices[count].getnext(deltaT);

		output.left += voiceOp.left;
		output.right += voiceOp.right;
	}

	output.left /= DIVEDER_PER_VOICE;
	output.right /= DIVEDER_PER_VOICE;

	// Apply reverb
#ifdef ENABLE_REVERB
	output = theReverb.getnext(output);
#endif

#ifdef ENABLE_OUTPUT_COMPRESSION
	// Each time the output reaches overflow, reduce level a bit

	// This is temp, compression does not adjust back up again

	output.left = output.left * compressionLevel;
	output.right = output.right * compressionLevel;

	if (output.left > 0.9F || output.right > 0.9F)
	{
		compressionLevel -= 0.01;
	}
#endif

	return output;
}

void synth::keyPressed(int midiKey, int midiVelocity)
{
	bool found = false;

	// first see if this key is already assigned to a voice, if it is retrigger it
	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		if (_voices[count].key == midiKey)
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
		nextVoice = 0;
	}
}


void synth::keyReleased(int midiKey)
{
	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		if (_voices[count].key == midiKey)
		{
			_voices[count].releasekey();
		}
	}
}

void synth::setPatch(int patchNumber)
{
	// Save the last select patch so if no sound comes out, it can be analysed
	char flieName[] = "Last_Patch.txt";
	SavePatch(this, _pSelectedPatch, flieName);

	_pSelectedPatch = &_allPatches[patchNumber];
	reconfigureSampleSet();	
}

void synth::reconfigureSampleSet()
{
	// Set up the samples sets
	for (int wgID=0; wgID<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; wgID++)
	{
		sampleSets[wgID].init(_pSelectedPatch->WGs[wgID].SampleSet.NSamples);

		printf("n samples = %i\n", _pSelectedPatch->WGs[wgID].SampleSet.NSamples);

		for (int count2=0; count2 < _pSelectedPatch->WGs[wgID].SampleSet.NSamples; count2++)
		{
			patchSampleSet sammpleSet = _pSelectedPatch->WGs[wgID].SampleSet;

			sampleSets[wgID].loadFile(sammpleSet.FileNames[count2],
										sammpleSet.SampledNoteFrequency[count2],
										sammpleSet.LoopStartSample[count2],
										sammpleSet.LoopEndSample[count2]);
		}
	}
}


void synth::InitalisePatches()
{
	fprintf(stderr, "Loading patches\n");

	char fileName[100];

	for (int count=0; count<128; count++)
	{
		try
		{
			sprintf(fileName, "patches/Patch_%i.txt", count);
			ReadPatchFromFile(&_allPatches[count], fileName);
		}
		catch (...)
		{
			fprintf(stderr, "Error reading patch file\n");
		}
	}

	fprintf(stderr, "Patches loaded\n");
}

