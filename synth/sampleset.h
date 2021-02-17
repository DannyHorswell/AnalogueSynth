#ifndef SAMPLESET_H
#define SAMPLESET_H

#include "wav.h"

class sampleset
{
	int nextToSet;

public:
	int _nSamples;
	wav* pWavArray;

	sampleset();
	~sampleset();

	void init(int nSamples);

	void loadFile(const string& FileName, float noteFrequency, int LoopStartSample, int LoopEndSample);
	wav* getWav(float freq);
};

#endif