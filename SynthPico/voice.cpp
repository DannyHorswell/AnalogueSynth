#include <stdio.h>
#include <string>
#include <vector>

#include "signal.h"
#include "lfo.h"
#include "constants.h"
#include "patch.h"
#include "keyPressedInfo.h"
#include "tdl.h"
#include "pan.h"
#include "noise.h"
#include "wavegenerator.h"
#include "LCRFilter.h"
#include "voice.h"
#include "synth.h"


voice::voice()
{
	//fprintf(stderr, "voice ctor\n");
}

voice::~voice()
{

}

void voice::init(signal deltaT)
{
	fprintf(stderr, "voice init\n");

	for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
	{
		waveGenerators[count].init(count);

#ifdef ENABLE_TDP
		TDPs[count].init(count, P);
#endif

#ifdef ENABLE_TDA
		TDAs[count].init(count, A);
#endif

	}

#ifdef ENABLE_TDF
		TDF.init(this, 0, F);
		theFilter.init(this, 0, deltaT);
		theFilter.recalculateCoefficients(); 
#endif
}



void voice::setkey(int midiKey, int midiVelocity)
{
	_keyPressedInfo.key = (float) midiKey;
	_keyPressedInfo.velocity = (float) midiVelocity;

	_keyPressedInfo.keyPressed = true;

	_keyPressedInfo.timeSincePressed = 0.0F;

	for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
	{
		printf("Count %i\n", count);
		waveGenerators[count].keyPress(_keyPressedInfo.velocity, _keyPressedInfo.key);
	}
}

void voice::releasekey()
{
	_keyPressedInfo.keyPressed = false;
	_keyPressedInfo.timeSinceReleased = 0.0F;
}


