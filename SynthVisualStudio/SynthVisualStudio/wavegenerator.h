#ifndef WAVEGENERATOR_H
#define WAVEGENERATOR_H

#include "constants.h"
#include "LCRFilter.h"
#include "pan.h"

/*******************************************************
* wavegeneratorclass
*
*
*
*
********************************************************/

class voice;


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

	pan thePan;
	stereo panMultipliers;

public:
	voice* _pVoice;
	LCRFilter theFilter;
	float keyFreq; // Picth the key plays (befor modification from pitch bend, LFO etc)

	wavegenerator();
	~wavegenerator();

	void init(synth* pSynth, voice* pVoice, int WGID,float deltaT);
	
	// Gets the next output value for the amount of time passed
	stereo getnext(float deltaT);

	void keyPress(float midiVelocity, float midiKey);

};





#endif

