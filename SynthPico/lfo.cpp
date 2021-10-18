#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>


#include "signal.h"
#include "constants.h"
#include "patch.h"


#include "lfo.h"
#include "noise.h"





lfo::lfo()
{
	//fprintf(stderr, "lfo ctor\n");
	lastFrequency = -1.0F;
}

lfo::~lfo()
{
}

void lfo::init(int LFOID)
{
	fprintf(stderr, "lfo init\n");

	_lfoID = LFOID;

	_part_period=0.0F;
}

signal lfo::getnext(signal deltaT)
{
#ifdef PRINT_GETNEXT
	printf("lfo::getnext\n");
#endif

	patchLFO* pLFO = &(GPatch.LFOs[_lfoID]);

	if (lastFrequency < sig_0 || lastFrequency != pLFO->frequency)
	{	
		lastFrequency = pLFO->frequency;
		_full_period = sig_1 / lastFrequency;
	}

	output = 0.0F;

	switch (pLFO->_type)
	{
		case waveformtype::SQUARE:
			if (_part_period < (_full_period / sig_2))
			{
				output = sig_NEG_1;
			}
			else
			{
				output = sig_1;
			}
			break;
	
		case waveformtype::SIN:
			output = wg_sin(sig_TWO_PI * _part_period / _full_period);
			break;

		case waveformtype::SAW:
			output = sig_NEG_1 + _part_period * 2.0F / _full_period;
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
					lastValue = sig_0-lastValue;
				}

				output = lastValue;
				sqLastPhase = true;
			}
			break;
	}

	_part_period += deltaT;

	if (_part_period >= _full_period)
	{
		_part_period = _part_period - _full_period;
	}

#ifdef PRINT_GETNEXT
	printf("end lfo::getnext\n");
#endif
	return output;
}

