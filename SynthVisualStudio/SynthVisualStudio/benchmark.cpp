#include <stdlib.h>
#include <stdio.h>
#include <iomanip>      // std::get_time
#include "constants.h"
#include "wavegenerator.h"
#include "voice.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"
#include "benchmark.h"

using namespace std;

benchmark::benchmark()
{
	deltaT = 1.0 / SAMPLE_FREQUENCY;

	start = NULL;
	stop = NULL;
	result = NULL;

	nOps = 100000;
}

benchmark::~benchmark()
{

}

float benchmark::testsynth()
{
	stereo out;

	printf("WG type = %i\n", theSynth._pSelectedPatch->WGs[0]._type);

	localtime(&start);

	theSynth.getnext(deltaT);

	for (int count=0; count<nOps; count++)
	{
		out = theSynth.getnext(deltaT);
	}

	localtime(&stop);

	double seconds = difftime(start, stop);

	float ret = ((float) nOps / (float)seconds);
	
	return ret;
}

float benchmark::testfilterspeed()
{
	float out;

	localtime(&start);

	theSynth._voices[0].setkey(64, 64);

	float in = 0.0;

	float maxOut = 0.0F;

	for (int count=0; count<nOps; count++)
	{
		// Generate a simple square wave in
		if ((count & 0x04) == 0x04)
		{
			if (in == 1.0)
			{
				in = - 1.0;
			}
			else
     		{
				in = 1.0;
			}
		}

		out = theSynth._voices[0].waveGenerators[0].theFilter.getNext(in);

		if (out > maxOut)
		{
			maxOut = out;
		}

		if (out < -maxOut)
		{
			maxOut = -out;
		}
	}

	printf("max = %f\n", maxOut);

	localtime(&stop);

	double seconds = difftime(start, stop);

	float ret = ((float)nOps / (float)seconds);

	return ret;
}

float benchmark::testfiltercoefficentcalculation()
{
	float out;

	localtime(&start);

	theSynth._voices[0].setkey(64, 64);

	for (int count=0; count<nOps; count++)
	{
		// Change something that forces recalculation
		theSynth._pSelectedPatch->WGs[0].Filter.Q = count & 0x0F; 
		theSynth._voices[0].waveGenerators[0].theFilter.recalculateCoefficients();
	}

	localtime(&stop);

	double seconds = difftime(start, stop);

	float ret = ((float)nOps / (float)seconds);

	return ret;
}

float benchmark::testwavegenerator()
{
	float out;

	printf("WG type = %i\n", theSynth._pSelectedPatch->WGs[0]._type);

	localtime(&start);

	theSynth._voices[0].setkey(64, 64);

	for (int count=0; count<nOps; count++)
	{
		out = theSynth._voices[0].waveGenerators[0].getnext(deltaT);
	}

	localtime(&stop);

	double seconds = difftime(start, stop);

	float ret = ((float)nOps / (float)seconds);

	return ret;
}

void benchmark::run()
{
	float opsPerSecond;

	theSynth.keyPressed(64, 64);

	//******  Test the speed of diferent wave types **************
	theSynth._pSelectedPatch->WGs[0]._type = MUTE;
	opsPerSecond = testwavegenerator();
	printf("WG MUTE Wave %f per second\n", opsPerSecond);

	theSynth._pSelectedPatch->WGs[0]._type = SQUARE;
	opsPerSecond = testwavegenerator();
	printf("WG SQUARE Wave %f per second\n", opsPerSecond);

	theSynth._pSelectedPatch->WGs[0]._type = SAW;
	opsPerSecond = testwavegenerator();
	printf("WG SAW Wave %f per second\n", opsPerSecond);

	theSynth._pSelectedPatch->WGs[0]._type = SIN;
	opsPerSecond = testwavegenerator();
	printf("WG SIN Wave %f permsecond\n", opsPerSecond);

	theSynth._pSelectedPatch->WGs[0]._type = NOISE;
	opsPerSecond = testwavegenerator();
	printf("WG NOISE Wave %f per second\n", opsPerSecond);

	theSynth._pSelectedPatch->WGs[0]._type = RND_SQ;
	opsPerSecond = testwavegenerator();
	printf("WG RND_SQ Wave %f per second\n", opsPerSecond);

	theSynth._pSelectedPatch->WGs[0]._type = PCM;
	opsPerSecond = testwavegenerator();
	printf("WG PCM Wave %f per second\n", opsPerSecond);

	// *********** Test Filter coefficient calculations
	opsPerSecond = testfiltercoefficentcalculation();
	printf("Filter coefficient recalc %f per second\n", opsPerSecond);

	// Test filter calculations
	opsPerSecond = testfilterspeed();
	printf("Filter speed %f per second\n", opsPerSecond);

	// Test synth
	theSynth._pSelectedPatch->WGs[0]._type = SQUARE;
	theSynth._pSelectedPatch->WGs[1]._type = SQUARE;

	opsPerSecond = testsynth();
	printf("Whole synth %f per second\n", opsPerSecond);
}