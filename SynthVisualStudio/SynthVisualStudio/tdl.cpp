#include <stdio.h>
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


const int TDL_RECALC_NUM = 512;

tdl::tdl()
{

}

tdl::~tdl()
{

}



void tdl::init(synth* pSynth, voice* pVoice, int wgID, tdlType type)
{
	_pSynth = pSynth;
	_pVoice = pVoice;
	_wgID = wgID;
	_type = type;
}

float tdl::getnext()
{
	patchTDL* ppatchTDL = NULL;

	switch (_type)
	{
#ifdef ENABLE_TDP
		case P:
			ppatchTDL = &_pSynth->_pSelectedPatch->WGs[_wgID].TDP;
			break;
#endif

#ifdef ENABLE_TDA
		case A:
			ppatchTDL = &_pSynth->_pSelectedPatch->WGs[_wgID].TDA;
			break;
#endif

#ifdef ENABLE_TDF
		case F:
			ppatchTDL = &_pSynth->_pSelectedPatch->WGs[_wgID].TDF;
			break;
#endif
	}
	if (ppatchTDL == NULL)
	{
		return 1.0F;
	}

	float amountThrough;
	float amountLeft;

	if (_pVoice->keyPressed)
	{
		if (lastKeystate == false)
		{
			pressLevel = output;
		}
		// In pressed phase

		// Is it between 0 and 1
		if (_pVoice->timeSincePressed <= ppatchTDL->T1)
		{
			amountThrough = _pVoice->timeSincePressed / ppatchTDL->T1;
			output = pressLevel +  (ppatchTDL->L1 -  pressLevel) * amountThrough;
		}

		// Is it between 1 and 2
		else if (_pVoice->timeSincePressed <= ppatchTDL->T1 + ppatchTDL->T2)
		{
			amountThrough = (_pVoice->timeSincePressed - ppatchTDL->T1) / ppatchTDL->T2;
			output = ppatchTDL->L1 + (ppatchTDL->L2 - ppatchTDL->L1) * amountThrough;
		}

		// Is it between 2 and 3
		else if (_pVoice->timeSincePressed <= ppatchTDL->T1 + ppatchTDL->T2 + ppatchTDL->T3)
		{
			amountThrough = (_pVoice->timeSincePressed - ppatchTDL->T1 - ppatchTDL->T2) / ppatchTDL->T3;
			output = ppatchTDL->L2 +  (ppatchTDL->Sustain - ppatchTDL->L2) * amountThrough;
		}
		else
 		{
			output = ppatchTDL->Sustain;
		}
		
		lastKeystate = true;
	}
	else
	{
		// In release phase
		if (lastKeystate)
		{
			// Key just release, record output level
			releaseLevel = output;
		}

		if (_pVoice->timeSinceReleased < ppatchTDL->T4)
		{
			amountLeft = (ppatchTDL->T4 - _pVoice->timeSinceReleased) / ppatchTDL->T4;
			amountThrough = 1.0F - amountLeft;

			output = ppatchTDL->L4 * amountThrough + releaseLevel * amountLeft;
		}
		else
		{
			output = ppatchTDL->L4;
		}
		
		lastKeystate = false;
	}

	if (_type == A)
	{
		// We want logarythmic for volume
		int index = (int) (output * VOLUME_MAPPING_STEPS);

		// Limit to 0 - 1 range
		index < 0 ? index = 0 : index;
		index > VOLUME_MAPPING_STEPS ? index = VOLUME_MAPPING_STEPS : index;

		output = pVolummeMapping[index];
	}

	return output;
}

void tdl::InitVolumnMapping()
{
	tdl::pVolummeMapping = new float[VOLUME_MAPPING_STEPS];

	float input = 0.0F;
	float output;

	for (int count = 0; count < VOLUME_MAPPING_STEPS; count++)
	{
		output = powf(input, 0.5F);
		tdl::pVolummeMapping[count] = output;
	}
}
