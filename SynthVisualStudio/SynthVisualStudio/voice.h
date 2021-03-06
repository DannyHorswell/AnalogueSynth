#ifndef VOICE_H
#define VOICE_H

#include "wavegenerator.h"
#include "tdl.h"

class voice
{
public:
	wavegenerator waveGenerators[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];

#ifdef ENABLE_TDP
	tdl TDPs[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];
#endif

#ifdef ENABLE_TDA
	tdl TDAs[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];
#endif

#ifdef ENABLE_TDF
	tdl TDFs[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];
#endif

	struct stereo output;

	synth* _pSynth;

	float key;
	float velocity;
	bool keyPressed;

	float timeSincePressed;
	float timeSinceReleased;

	float tvlDeltaT;
	int tvlRecalcCount;

	voice();
	~voice();

	void init(synth* pSynth, float deltaT);

	stereo getnext(float deltaT);

	void setkey(int midiKey, int midiVelocity);
	void releasekey();
};

#endif