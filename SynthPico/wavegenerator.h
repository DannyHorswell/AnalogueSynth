#pragma once




/*******************************************************
* wavegeneratorclass
*
*
*
*
********************************************************/

const int RECALC_COUNT = 16;

class wavegenerator
{
	int _wgID;

	// recalc freq count
	int relcalcPeriodCount;

	// Where in the phase (0 - 2PI) the cycle of teh wave is
	float _full_period; // Time for a full cycle
	float _part_period; // Amount so far through period
	
	// For rnd squ
	bool sqLastPhase;
	signal lastValue;

	signal lastKey;
	signal waveformLevel;	// Calculated from key velocity and patch

	pan thePan;
	

public:

	signal keyFreq; // Picth the key plays (befor modification from pitch bend, LFO etc)

	signal lastOutput;

	wavegenerator();
	~wavegenerator();

	void init(int WGID);
	
	stereo panMultipliers;

	void keyPress(signal midiVelocity, signal midiKey);

	inline signal getnext(float deltaT, keyPressedInfo kpi, tdl TDP)
{
#ifdef PRINT_GETNEXT
	printf("wavegenerator::getnext\n");
#endif

	patchWG* patchWG = &GPatch.WGs[_wgID];

	if (kpi.key == 0.0 || patchWG->_type == waveformtype::MUTE)
	{
		lastOutput = sig_0;
		return lastOutput;
	}

	if (relcalcPeriodCount++ == 0)
	{	
		//printf("keyOffestSemitones: %f\n", patchWG->keyOffestSemitones.ToFloat());
		keyFreq = signal(kpi.key) + patchWG->keyOffestSemitones;

		signal calcKey = keyFreq;

		if (patchWG->enablePitchBend)
		{
			calcKey += GSynth._pitchBendSemitones * patchWG->pitchBendAmount;
		}

		// LFO
		if (kpi.timeSincePressed > patchWG->freqLFODelay)
		{
			calcKey += GSynth._LFOs[patchWG->freqLFOid].output * patchWG->freqLFOLevel;
		}


#ifdef ENABLE_TDP
		calcKey += TDP.output;
#endif

 		_full_period = GSynth.FrequencyToPeriod(GSynth.KeyNumberToFrequency(calcKey));
	}

	if (relcalcPeriodCount >= RECALC_COUNT)
	{
		relcalcPeriodCount = 0; // Recalc next time
	}

	signal mono = 0.0F;

	float mid = _full_period / 2;

	mid += (GSynth._LFOs[patchWG->pwmLFOid].output * patchWG->pwmLFOLevel).ToFloat() * mid;

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
				mono = sig_0-waveformLevel;
			}
			else
			{
				mono = waveformLevel;
			}
			break;
	
		case waveformtype::SIN:
			mono = waveformLevel * wg_sin(sig_TWO_PI * _part_period / _full_period);
			break;

		case waveformtype::SAW:
		
			mono = waveformLevel * signal(1 + _part_period * 2 / _full_period);

			
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
					lastValue = sig_0-lastValue;
				}

				mono = lastValue * waveformLevel;
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
	printf("%f\n", mono);
#endif

	lastOutput = mono;

	return lastOutput;
}




};





