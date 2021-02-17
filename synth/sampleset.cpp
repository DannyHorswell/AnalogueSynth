#include <limits> 

#include "sampleset.h"

using namespace std;

sampleset::sampleset()
{
	printf("sampleset::ctor\n");
	_nSamples = -1;
	pWavArray = NULL;
}

sampleset::~sampleset()
{
	delete[] pWavArray;
}

void sampleset::init(int nSamples)
{
	printf("sampleset::init %i samples\n", nSamples);

	if (_nSamples != nSamples)
	{
		if (pWavArray)
		{
			delete[] pWavArray;
		}

		_nSamples = nSamples;
		pWavArray = new	wav[nSamples];

		printf("wav array constructed\n");
	}

	nextToSet = 0;
}

// Loads the next wav
void sampleset::loadFile(const string& FileName, float noteFrequency, int LoopStartSample, int LoopEndSample)
{
	if (nextToSet < _nSamples)
	{
		pWavArray[nextToSet++].loadFile(FileName, noteFrequency, LoopStartSample, LoopEndSample);
	}

	//printf("nextToSet = %i\n", nextToSet);
}

int lastClosest = -1;

wav* sampleset::getWav(float freq)
{
	// Get the closest frequency to play
	int closest = 0;
	float bestDiff = numeric_limits<float>::max();

	for (int count=0; count<nextToSet; count++)
	{
		float diff = pWavArray[count].sampledNoteFrequency - freq;

		if (diff < 0.0F)
		{
			diff = 0.0F - diff;
		}

		if (diff < bestDiff)
		{
			bestDiff = diff;
			closest = count;
		}
	}

	//if (closest != lastClosest)
	//{
	//	lastClosest = closest;

	//	printf("closest = %i\n", closest);
	//}

	return &pWavArray[closest];
}