#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "signal.h"
#include "constants.h"
#include "patch.h"

#include "reverb.h"


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

void reverb::init()
{
}

stereo reverb::getnext(stereo input)
{
	input.right = input.right / 2.0F;
	input.left = input.left / 2.0F;

	stereo output;

	if (pBuffer == NULL || !GPatch.Reverb.enabled)
	{
		output = input;
	}
	else
	{
		signal feedback = GPatch.Reverb.feedback;
		signal level = GPatch.Reverb.level;

		// Crossover left right
		pBuffer[nextSample].left = (input.right + pBuffer[nextSample].right) * feedback;
		pBuffer[nextSample].right = (input.left + pBuffer[nextSample].left) * feedback;

		nextSample++;

		if (nextSample >= GPatch.Reverb.sampleLength ||
			nextSample >= MAX_REVERB_SAMPLES)
		{
			nextSample = 0;
		}
	
		output.left = input.left + (pBuffer[nextSample].left * level);
		output.right = input.right + (pBuffer[nextSample].right * level);
	}

	return output;
}
