#ifndef WAV_H
#define WAV_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;

class wav
{
	float* pSampleBuffer;
	float sampledNotePeriod;

public:
	string fileName;

	int nSamples;
	float sampleRate;
	float samplePeriod;
	float sampleLengthTime;
	float sampledNoteFrequency;

	int _loopStartSample;
	int _loopEndSample;

	wav();
	~wav();

	// If loopEndSample = 0, no loop
	void loadFile(const string& FileName, float noteFrequency, int loopStartSample, int loopEndSample);
	float getNext(float& positionTime, float deltaT, float _full_period, bool keyPressed);
};

#endif