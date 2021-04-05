#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>


#include "constants.h"
#include "patch.h"
#include "LCRFilter.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "voice.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"
#include "noise.h"


lfo::lfo()
{
	//fprintf(stderr, "lfo ctor\n");
	lastFrequency = -1.0F;
}

lfo::~lfo()
{
}

void lfo::init(synth* pSynth, int LFOID)
{
	fprintf(stderr, "lfo init\n");

	_pSynth = pSynth;
	_lfoID = LFOID;

	_part_period=0.0F;
}

float lfo::getnext(float deltaT)
{
#ifdef PRINT_GETNEXT
	printf("lfo::getnext\n");
#endif

	patchLFO* pLFO = &(_pSynth->_pSelectedPatch->LFOs[_lfoID]);

	if (lastFrequency < 0.0 || lastFrequency != pLFO->frequency)
	{	
		lastFrequency = pLFO->frequency;
		_full_period = 1.0F / lastFrequency;
	}

	output = 0.0F;

	switch (pLFO->_type)
	{
		case waveformtype::SQUARE:
			if (_part_period < (_full_period / 2.0F))
			{
				output = -1.0F;
			}
			else
			{
				output = 1.0F;
			}
			break;
	
		case waveformtype::SIN:
			output = wg_sin(TWO_PI * _part_period / _full_period);
			break;

		case waveformtype::SAW:
			output = -1.0F + _part_period * 2.0F / _full_period;
			break;

		case waveformtype::NOISE:
			output = theNoiseGenerator.getnext();
			break;

		case waveformtype::RND_SQ:
			if (_part_period < (_full_period / 2.0F))
			{
				if (sqLastPhase)
				{
					lastValue = -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
				}
	
				output = lastValue;
				sqLastPhase = false;
			}
			else
			{
				if (!sqLastPhase)
				{
					lastValue = -lastValue;
				}

				output = lastValue;
				sqLastPhase = true;
			}
			break;
	}

	_part_period += deltaT;

	if (_part_period >= _full_period)
	{
		_part_period -= _full_period;
	}

#ifdef PRINT_GETNEXT
	printf("end lfo::getnext\n");
#endif
	return output;
}

