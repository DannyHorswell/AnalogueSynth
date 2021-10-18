#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>


#include "constants.h"
#include "patch.h"
#include "wavegenerator.h"






wavegenerator::wavegenerator()
{
	//fprintf(stderr, "wavegenerator ctor\n");
}

wavegenerator::~wavegenerator()
{
}

void wavegenerator::init(int wgID)
{
	fprintf(stderr, "wavegenerator init\n");

	_wgID = wgID;

	_part_period=0.0F;
}

void wavegenerator::keyPress(const signal midiVelocity, const signal midiKey)
{
	printf("WG key Press");

	patchWG* patchWG = & GPatch.WGs[_wgID];

	signal velocityNegHalftoHalf = (midiVelocity - sig_64) / sig_128;

	signal keyNegOnetoOne = (midiKey - sig_64) / sig_64;

	waveformLevel = sig_HALF + (patchWG->velocityVolumeAdjust) * velocityNegHalftoHalf;

	if (waveformLevel < 0.0F)
	{
		waveformLevel = 0.0F;
	}

	if (waveformLevel > 1.0F)
	{
		waveformLevel = 1.0F;
	}

	signal panLevel = sig_0;


	panLevel = panLevel + patchWG->fixedPanAdjustment;
	//panLevel += 0.5F + (patchWG->velocityPanAdjust) * velocityNegHalftoHalf;
	panLevel = panLevel + (patchWG->keyPanAdjustment) * keyNegOnetoOne;
	panMultipliers = thePan.GetStereoMultipliers(panLevel);
	//printf("keyNegHalftoHalf %f, pan level %f\n", keyNegOnetoOne, panLevel);
}

