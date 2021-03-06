#include <stdio.h>
#include <stdlib.h>
#include <string>
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


reverb::reverb()
{
	fprintf(stderr, "reverb ctor");

	pBuffer = (stereo*) malloc(sizeof(stereo) * MAX_REVERB_SAMPLES);

	if (pBuffer == NULL)
	{
		fprintf(stderr, "Could not allocate reverb buffer");
	}
}

reverb::~reverb()
{
	free(pBuffer);
}

void reverb::init(synth* pSynth)
{
	_pSynth = pSynth;
}

stereo reverb::getnext(stereo input)
{
	patchReverb* pRevPatch = &_pSynth->_pSelectedPatch->Reverb;
	
	

	input.right = input.right / 2.0F;
	input.left = input.left / 2.0F;

	stereo output;

	if (pBuffer == NULL || !pRevPatch->enabled)
	{
		output = input;
	}
	else
	{
		float feedback = pRevPatch->feedback;
		float level = pRevPatch->level;

		// Crossover left right
		pBuffer[nextSample].left = (input.right + pBuffer[nextSample].right) * feedback;
		pBuffer[nextSample].right = (input.left + pBuffer[nextSample].left) * feedback;

		nextSample++;

		if (nextSample >= pRevPatch->sampleLength ||
			nextSample >= MAX_REVERB_SAMPLES)
		{
			nextSample = 0;
		}
	
		output.left = input.left + (pBuffer[nextSample].left * level);
		output.right = input.right + (pBuffer[nextSample].right * level);
	}

	return output;
}
