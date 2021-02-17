#include <stdio.h>
#include <string>
#include <vector>

#include "constants.h"
#include "patch.h"
#include "filter.h"
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
	patchTDL* ppatchTDL;

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

	float amountThrough;

	if (_pVoice->keyPressed)
	{
		// In pressed phase

		// Is it between 0 and 1
		if (_pVoice->timeSincePressed <= ppatchTDL->T1)
		{
			amountThrough = _pVoice->timeSincePressed / ppatchTDL->T1;
			output = ppatchTDL->L0 +  (ppatchTDL->L1 -  ppatchTDL->L0) * amountThrough;
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
	}
	else
	{
		// In release phase

		if (_pVoice->timeSinceReleased < ppatchTDL->T4)
		{
			amountThrough = (ppatchTDL->T4 - _pVoice->timeSinceReleased) / ppatchTDL->T4;
			output = ppatchTDL->L4 + (output - ppatchTDL->L4) * amountThrough;
		}
		else
		{
			output = ppatchTDL->L4;
		}
	}

	return output;
}