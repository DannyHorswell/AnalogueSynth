#pragma once

#include <math.h>



const int LFO_RECALC_NUM = 512;

extern signal sig_dividerPerVoice;

class synth
{
	int nextVoice;

	float _keyFreqMultiplier; // Calculated from master tune
	float _masterTune;
	
	signal lfoDeltaT;
	int lfoRecalcCount;

#ifdef ENABLE_REVERB
	reverb theReverb;
#endif

	int _selectedPatch;

	
	
public:

	voice _voices[NUMBER_OF_VOICES];
	lfo _LFOs[NUMBER_OF_LFO_FOR_SYNTH];

	signal _pitchBendSemitones;

	stereo output;

	synth();
	~synth();

	void InitalisePatches();
	void setMasterTune(float freq);
	
	inline float KeyNumberToFrequency(const signal keyNumber)
	{
		signal keyToOct(keyNumber / sig_12);
		float octivePos = keyToOct.ToFloat();

		float freq = powf( 2.0F, octivePos) * _keyFreqMultiplier;

		return freq;
	}

	inline float FrequencyToPeriod(const float freq)
	{
		float period = 1.0F / freq;
		return period;
	}

#ifdef GENERATE_TEST_SOUND
	signal getNextTestValue();
#endif


inline stereo getnext(const float deltaT)
{
#ifdef PRINT_GETNEXT
	printf("synth::getnext\n");
#endif


	// Recalculate LFOs
	if (lfoRecalcCount++ == 0)
	{
		for (int count=0; count<NUMBER_OF_LFO_FOR_SYNTH; count++)
		{
			signal val =_LFOs[count].getnext(lfoDeltaT);
		}
		
		lfoDeltaT = sig_0;
	}
	else
	{
		lfoDeltaT += deltaT;
	}

	if (lfoRecalcCount >= LFO_RECALC_NUM)
	{
		lfoRecalcCount = 0;
	}

	// Mixes all voices
	output.left = 0.0F;
	output.right = 0.0F;

	for (int count=0; count<NUMBER_OF_VOICES; count++)
	{
		stereo voiceOp = _voices[count].getnext(deltaT);

		output.left += voiceOp.left;
		output.right += voiceOp.right;
	}

	output.left /= sig_dividerPerVoice;
	output.right /= sig_dividerPerVoice;

	return output;

	// Apply reverb
#ifdef ENABLE_REVERB
	output = theReverb.getnext(output);
#endif

#ifdef GENERATE_TEST_SOUND

	output.left = getNextTestValue();
	output.right = output.left;
#endif

	return output;
}

	void keyPressed(int midiKey, int midiVelocity);
	void keyReleased(int midiKey);

	void setPatch(int patchNo);
};


