#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>


#include "constants.h"
#include "patch.h"
#include "filter.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "voice.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"
#include "noise.h"



const int RECALC_COUNT = 16;

wavegenerator::wavegenerator()
{
	//fprintf(stderr, "wavegenerator ctor\n");
}

wavegenerator::~wavegenerator()
{
}

void wavegenerator::init(synth* pSynth, voice* pVoice, int wgID)
{
	fprintf(stderr, "wavegenerator init\n");

	_pSynth = pSynth;
	_pVoice = pVoice;
	_wgID = wgID;

	_part_period=0.0F;

	theFilter.init(pSynth, wgID, this);
}

void wavegenerator::keyPress(float velocity)
{
	pcmTime = 0.0F;

	patchWG* patchWG = &_pSynth->_pSelectedPatch->WGs[_wgID];

	waveformLevel = 0.5F + (velocity - 64.0F) * (patchWG->velocityVolumeAdjust) / 128.0F;

	if (waveformLevel < 0.0F)
	{
		waveformLevel = 0.0F;
	}

	if (waveformLevel > 1.0F)
	{
		waveformLevel = 1.0F;
	}

	//printf("waveform level%f\n", waveformLevel);
}

float wavegenerator::getnext(float deltaT)
{
#ifdef PRINT_GETNEXT
	printf("wavegenerator::getnext\n");
#endif

	patchWG* patchWG = &_pSynth->_pSelectedPatch->WGs[_wgID];

	if (_pVoice->key == 0.0 || patchWG->_type == MUTE)
	{
		//printf("key mute\n");
		return 0.0F;
	}

	//printf("no mute\n");

	if (relcalcPeriodCount++ == 0)
	{	
		keyFreq = _pVoice->key + patchWG->keyOffestSemitones;

		float calcKey = keyFreq;

		if (patchWG->enablePitchBend)
		{
			calcKey += _pVoice->_pSynth->_pitchBendSemitones * patchWG->pitchBendAmount;
		}

		// LFO
		if (_pVoice->timeSincePressed > patchWG->freqLFODelay)
		{
			calcKey += _pVoice->_pSynth->_LFOs[patchWG->freqLFOid].output * patchWG->freqLFOLevel;
		}

#ifdef ENABLE_TDP
		calcKey += _pVoice->TDPs[_wgID].output;
#endif
 		_full_period = _pVoice->_pSynth->KeyToPeriod(calcKey);

		#ifdef ENABLE_FILTERS

		//if (_pVoice->key != lastKey)
		//{
			theFilter.recalculateCoefficients(); // In case things have changed
			lastKey = _pVoice->key;
		//}

	#endif
	}

	if (relcalcPeriodCount >= RECALC_COUNT)
	{
		relcalcPeriodCount = 0; // Recalc next time
	}

	float ret = 0.0F;

	float mid = _full_period / 2.0F;

	mid += _pVoice->_pSynth->_LFOs[patchWG->pwmLFOid].output * patchWG->pwmLFOLevel * _full_period / 2.0F;

	if (mid < 0.0F)
	{
		mid = 0.0F;
	}


	switch (patchWG->_type)
	{
		case MUTE:
			ret = 0.0F;
			break;

		case SQUARE:
			if (_part_period < mid)
			{
				ret = -waveformLevel;
			}
			else
			{
				ret = waveformLevel;
			}
			break;
	
		case SIN:
			ret = waveformLevel * wg_sin(TWO_PI * _part_period / _full_period);
			break;

		case SAW:
			ret = waveformLevel * (-1.0F + _part_period * 2.0F / _full_period) ;
			break;

		case NOISE:
			ret = waveformLevel * theNoiseGenerator.getnext();
			break;

		case RND_SQ:
			if (_part_period < (_full_period / 2.0F))
			{
				if (sqLastPhase)
				{
					lastValue = -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
				}
	
				ret = lastValue;
				sqLastPhase = false;
			}
			else
			{
				if (!sqLastPhase)
				{
					lastValue = -lastValue;
				}

				ret = lastValue * waveformLevel;
				sqLastPhase = true;
			}
			break;

		case PCM:
			ret = waveformLevel * _pSynth->sampleSets[_wgID].getWav(_pSynth->KeyNumberToFrequency(keyFreq))->getNext(pcmTime, deltaT, _full_period, _pVoice->keyPressed);
			break;
	}

	_part_period += deltaT;

	if (_part_period >= _full_period)
	{
		_part_period -= _full_period;
	}

#ifdef ENABLE_FILTERS
	ret = theFilter.getNext(ret); // Apply the filter
#endif

#ifdef PRINT_GETNEXT
	printf("%f\n", ret);
#endif

	return ret;
}


