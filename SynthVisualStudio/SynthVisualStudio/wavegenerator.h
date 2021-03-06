#ifndef WAVEGENERATOR_H
#define WAVEGENERATOR_H

#include "LCRFilter.h"

/*******************************************************
* wavegeneratorclass
*
*
*
*
********************************************************/

class voice;

#define B_ 1.2732395447 //B = 4.0 / PI;
#define C_ -0.405284735 //C = -4.0 /(PI_SQUARED);

class wavegenerator
{
	synth* _pSynth;
	int _wgID;

	// recalc freq count
	int relcalcPeriodCount;

	// Where in the phase (0 - 2PI) the cycle of teh wave is
	float _full_period; // Time for a full cycle
	float _part_period; // Amount so far through period
	
	// For rnd squ
	bool sqLastPhase;
	float lastValue;

	float lastKey;
	float waveformLevel;	// Calculated from key velocity and patch

	float pcmTime;

public:
	voice* _pVoice;
	LCRFilter theFilter;
	float keyFreq; // Picth the key plays (befor modification from pitch bend, LFO etc)

	wavegenerator();
	~wavegenerator();

	void init(synth* pSynth, voice* pVoice, int WGID,float deltaT);
	
	// Gets the next output value for the amount of time passed
	float getnext(float deltaT);

	void keyPress(float velocity);

};

inline float wg_sin(float x)
	{
		if (x > PI)
		{
			x -= TWO_PI;
		}

		return -(B_ * x + C_ * x * ((x < 0) ? -x : x));
	}



#endif

