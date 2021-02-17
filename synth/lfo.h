#ifndef LFO_H
#define LFO_H

/*******************************************************
* lfo class
*
*
*
*
********************************************************/

class voice;

class lfo
{
	synth* _pSynth;
	struct patch* _pPatch;
	int _lfoID;

	// Where in the phase (0 - 2PI) the cycle of the wave is
	float _full_period; // Time for a full cycle
	float _part_period; // Amount so far through period
	
	// For rnd squ
	bool sqLastPhase;
	float lastValue;

	float lastFrequency;
public:
	float output; // last output value

	lfo();
	~lfo();

	void init(synth* pSynth, int LFOID);
	
	// Gets the next output value for the amount of time passed
	float getnext(float deltaT);
};

#endif

