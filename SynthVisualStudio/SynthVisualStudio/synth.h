#ifndef SYNTH_H
#define SYNTH_H

#include "voice.h"
#include "reverb.h"
#include "wav.h"
#include "sampleset.h"
#include "patch.h"

class synth
{
	int nextVoice;

	float _keyFreqMultiplier; // Calculated from master tune
	float _masterTune;
	
	float lfoDeltaT;
	int lfoRecalcCount;

	reverb theReverb;

	int _selectedPatch;

	void InitalisePatches();
	
public:
	voice _voices[NUMBER_OF_VOICES];
	sampleset sampleSets[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];
	lfo _LFOs[NUMBER_OF_LFO_FOR_SYNTH];

	patch _allPatches[128];
	patch* _pSelectedPatch;

	float _pitchBendSemitones;

	stereo output;

	synth(float deltaT);
	~synth();

	void setMasterTune(float freq);
	
	float KeyNumberToFrequency(float keyNumber);
	float KeyToPeriod(float keyNumber);

	stereo getnext(float deltaT);

	void keyPressed(int midiKey, int midiVelocity);
	void keyReleased(int midiKey);

	void setPatch(int patchNo);
	void reconfigureSampleSet();
};

#endif