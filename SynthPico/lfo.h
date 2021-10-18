#pragma once

/*******************************************************
* lfo class
*
*
*
*
********************************************************/

//class voice;

class lfo
{
	struct patch* _pPatch;
	int _lfoID;

	// Where in the phase (0 - 2PI) the cycle of the wave is
	signal _full_period; // Time for a full cycle
	signal _part_period; // Amount so far through period
	
	// For rnd squ
	bool sqLastPhase;
	signal lastValue;

	signal lastFrequency;
public:
	signal output; // last output value

	lfo();
	~lfo();

	void init(int LFOID);
	
	// Gets the next output value for the amount of time passed
	signal getnext(signal deltaT);
};


