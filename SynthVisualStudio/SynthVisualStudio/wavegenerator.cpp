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



const int RECALC_COUNT = 16;

wavegenerator::wavegenerator()
{
	//fprintf(stderr, "wavegenerator ctor\n");
}

wavegenerator::~wavegenerator()
{
}

void wavegenerator::init(synth* pSynth, voice* pVoice, int wgID, float deltaT)
{
	fprintf(stderr, "wavegenerator init\n");

	_pSynth = pSynth;
	_pVoice = pVoice;
	_wgID = wgID;

	_part_period=0.0F;

	theFilter.init(pSynth, pVoice, wgID, this, deltaT);
}

void wavegenerator::keyPress(float midiVelocity, float midiKey)
{
	pcmTime = 0.0F;

	patchWG* patchWG = &_pSynth->_pSelectedPatch->WGs[_wgID];
	float velocityNegHalftoHalf = (midiVelocity - 64.0F) / 128.0F;
	float keyNegOnetoOne = (midiKey - 64.0F) / 64.0F;

	waveformLevel = 0.5F + (patchWG->velocityVolumeAdjust) * velocityNegHalftoHalf;

	if (waveformLevel < 0.0F)
	{
		waveformLevel = 0.0F;
	}

	if (waveformLevel > 1.0F)
	{
		waveformLevel = 1.0F;
	}

	float panLevel = 0.0;

	panLevel += patchWG->fixedPanAdjustment;
	//panLevel += 0.5F + (patchWG->velocityPanAdjust) * velocityNegHalftoHalf;
	panLevel += (patchWG->keyPanAdjustment) * keyNegOnetoOne;

	panMultipliers = thePan.GetStereoMultipliers(panLevel);
	//printf("keyNegHalftoHalf %f, pan level %f\n", keyNegOnetoOne, panLevel);
}

stereo wavegenerator::getnext(float deltaT)
{
#ifdef PRINT_GETNEXT
	printf("wavegenerator::getnext\n");
#endif

	stereo ret;

	patchWG* patchWG = &_pSynth->_pSelectedPatch->WGs[_wgID];

	if (_pVoice->key == 0.0 || patchWG->_type == waveformtype::MUTE)
	{
		ret.left = 0.0F;
		ret.right = 0.0F;
		return ret;
	}

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

		theFilter.recalculateCoefficients(); // In case things have changed
		lastKey = _pVoice->key;
#endif
	}

	if (relcalcPeriodCount >= RECALC_COUNT)
	{
		relcalcPeriodCount = 0; // Recalc next time
	}

	float mono = 0.0F;

	float mid = _full_period / 2.0F;

	mid += _pVoice->_pSynth->_LFOs[patchWG->pwmLFOid].output * patchWG->pwmLFOLevel * _full_period / 2.0F;

	if (mid < 0.0F)
	{
		mid = 0.0F;
	}

	switch (patchWG->_type)
	{
		case waveformtype::MUTE:
			mono = 0.0F;
			break;

		case waveformtype::SQUARE:
			if (_part_period < mid)
			{
				mono = -waveformLevel;
			}
			else
			{
				mono = waveformLevel;
			}
			break;
	
		case waveformtype::SIN:
			mono = waveformLevel * wg_sin(TWO_PI * _part_period / _full_period);
			//ret = waveformLevel * sin(TWO_PI * _part_period / _full_period);
			break;

		case waveformtype::SAW:
			mono = waveformLevel * (-1.0F + _part_period * 2.0F / _full_period) ;
			break;

		case waveformtype::NOISE:
			mono = waveformLevel * theNoiseGenerator.getnext();
			break;

		case waveformtype::RND_SQ:
			if (_part_period < (_full_period / 2.0F))
			{
				if (sqLastPhase)
				{
					lastValue = -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
				}
	
				mono = lastValue;
				sqLastPhase = false;
			}
			else
			{
				if (!sqLastPhase)
				{
					lastValue = -lastValue;
				}

				mono = lastValue * waveformLevel;
				sqLastPhase = true;
			}
			break;

		case waveformtype::PCM:
			mono = waveformLevel * _pSynth->sampleSets[_wgID].getWav(_pSynth->KeyNumberToFrequency(keyFreq))->getNext(pcmTime, deltaT, _full_period, _pVoice->keyPressed);
			break;
	}

	_part_period += deltaT;

	if (_part_period >= _full_period)
	{
		_part_period -= _full_period;
	}

#ifdef ENABLE_FILTERS
	mono = theFilter.getNext(mono); // Apply the filter
#endif

#ifdef PRINT_GETNEXT
	printf("%f\n", mono);
#endif

	ret.left = mono * panMultipliers.left;
	ret.right = mono * panMultipliers.right;

	return ret;
}


