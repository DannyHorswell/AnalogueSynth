#include <stdlib.h>
#include <stdio.h>
#include <iomanip>      // std::get_time
#include "constants.h"
#include "voice.h"
#include "wavegenerator.h"

#include "lfo.h"
#include "reverb.h"
#include "synth.h"
#include "benchmark.h"

using namespace std;

benchmark::benchmark()
{
	deltaT = 1.0 / SAMPLE_FREQUENCY;

	pTheSynth = new synth(deltaT);

	start = 0;
	stop = 0;
	result = 0;

	nOps = 100000;
}

benchmark::~benchmark()
{
	delete pTheSynth;
}

float benchmark::testsynth()
{
	stereo out;

	printf("WG type = %i\n", pTheSynth->_pSelectedPatch->WGs[0]._type);

	localtime(&start);

	pTheSynth->getnext(deltaT);

	for (int count=0; count<nOps; count++)
	{
		out = pTheSynth->getnext(deltaT);
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

	pTheSynth->_voices[0].setkey(64, 64);

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

		out = pTheSynth->_voices[0].waveGenerators[0].theFilter.getNext(in);

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

	pTheSynth->_voices[0].setkey(64, 64);

	for (int count=0; count<nOps; count++)
	{
		// Change something that forces recalculation
		pTheSynth->_pSelectedPatch->WGs[0].Filter.Q = count & 0x0F; 
		pTheSynth->_voices[0].waveGenerators[0].theFilter.recalculateCoefficients();
	}

	localtime(&stop);

	double seconds = difftime(start, stop);

	float ret = ((float)nOps / (float)seconds);

	return ret;
}

float benchmark::testwavegenerator()
{
	float sample;
	stereo out;

	printf("WG type = %i\n", pTheSynth->_pSelectedPatch->WGs[0]._type);

	localtime(&start);

	pTheSynth->_voices[0].setkey(64, 64);

	for (int count=0; count<nOps; count++)
	{
		sample = pTheSynth->_voices[0].waveGenerators[0].getnext(deltaT, 0.0F);
		
		out.left = pTheSynth->_voices[0].waveGenerators[0].panMultipliers.left * sample;
		out.right = pTheSynth->_voices[0].waveGenerators[0].panMultipliers.right * sample;
	}

	localtime(&stop);

	double seconds = difftime(start, stop);

	float ret = ((float)nOps / (float)seconds);

	return ret;
}

void benchmark::run()
{
	float opsPerSecond;

	pTheSynth->keyPressed(64, 64);

	//******  Test the speed of diferent wave types **************
	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::MUTE;
	opsPerSecond = testwavegenerator();
	printf("WG MUTE Wave %f per second\n", opsPerSecond);

	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::SQUARE;
	opsPerSecond = testwavegenerator();
	printf("WG SQUARE Wave %f per second\n", opsPerSecond);

	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::SAW;
	opsPerSecond = testwavegenerator();
	printf("WG SAW Wave %f per second\n", opsPerSecond);

	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::SIN;
	opsPerSecond = testwavegenerator();
	printf("WG SIN Wave %f permsecond\n", opsPerSecond);

	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::NOISE;
	opsPerSecond = testwavegenerator();
	printf("WG NOISE Wave %f per second\n", opsPerSecond);

	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::RND_SQ;
	opsPerSecond = testwavegenerator();
	printf("WG RND_SQ Wave %f per second\n", opsPerSecond);

	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::PCM;
	opsPerSecond = testwavegenerator();
	printf("WG PCM Wave %f per second\n", opsPerSecond);

	// *********** Test Filter coefficient calculations
	opsPerSecond = testfiltercoefficentcalculation();
	printf("Filter coefficient recalc %f per second\n", opsPerSecond);

	// Test filter calculations
	opsPerSecond = testfilterspeed();
	printf("Filter speed %f per second\n", opsPerSecond);

	// Test synth
	pTheSynth->_pSelectedPatch->WGs[0]._type = waveformtype::SQUARE;
	pTheSynth->_pSelectedPatch->WGs[1]._type = waveformtype::SQUARE;

	opsPerSecond = testsynth();
	printf("Whole synth %f per second\n", opsPerSecond);
}
