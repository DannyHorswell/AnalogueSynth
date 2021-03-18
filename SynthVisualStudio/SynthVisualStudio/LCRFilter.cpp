
#include <math.h>

#include "constants.h"
#include "patch.h"
#include "LCRFilter.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "voice.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"


void LCRFilter::init(synth* pSynth, voice* pVoice, int wgID, wavegenerator* pWG, float deltaT)
{
	_pSynth = pSynth;
    _pVoice = pVoice;
	_wgID = wgID;
	_pWG = pWG;

    _deltaT = deltaT;
}

/// <summary>
/// These need recalculating when f and q change
/// </summary>
/// <param name="frequency"></param>
/// <param name="q"></param>
void LCRFilter::RecalculateValues(float frequency, float q)
{
    C = 1.0F / (TWO_PI * frequency);
    C = C * C;

    R = sqrt(1.0F / (C * q));

    Alpha = R / 2.0F;
    Beta = sqrt((1.0F / C) + (R * R) / 4.0F);

    double exponentBit = exp(-Alpha * _deltaT);
    double sinBit = sin(Beta * _deltaT);
    double cosBit = cos(Beta * _deltaT);

    VzeroComponent = (exponentBit * sinBit) / Beta;
    IzeroComponent = exponentBit * cosBit;
}


void LCRFilter::recalculateCoefficients()
{
    patchFilter* pPatchFilter = &_pSynth->_pSelectedPatch->WGs[_wgID].Filter;

    float key = _pWG->_pVoice->key + ((_pWG->_pVoice->velocity) / 64.0F) * pPatchFilter->VelocityFactor;
    key += _pVoice->TDFs[_wgID].output;

    float freq = _pWG->_pVoice->_pSynth->KeyNumberToFrequency(key) * pPatchFilter->RelativeFrequency;

    if (freq > 12000.0F)
    {
        freq = 12000.0F;
    }

    float Q =  pPatchFilter->Q;

    RecalculateValues(freq, Q);


    
}


/// <summary>
/// Get the next sample
/// </summary>
/// <param name="x">input</param>
/// <returns>output</returns>
float LCRFilter::getNext(float x)
{
    float vdiff;
    float i;
    float dv;

    patchFilter* pPatchFilter = &_pSynth->_pSelectedPatch->WGs[_wgID].Filter;

    switch (pPatchFilter->FilterType)
	{
		case OFF:
            return x;
            break;

		case LPF:

            vdiff = x - Vc;

            i = vdiff * VzeroComponent + I * IzeroComponent;

            // Store I for next iteration
            I = i;

            // Calculate diference in Vc
            dv = (I * _deltaT) / C;

            // Store Vc for next iteration
            Vc += dv;

            return Vc;
            break;

		case HPF:
            return x;
            break;

		case BPF1:
                    return x;
            break;
		case BPF2:
                    return x;
            break;
		case NOTCH:
                    return x;
            break;
		case APF:
                    return x;
            break;
		case PEAKINGEQ:
             return x;
            break;
		case LOWSHELF:
            return x;
            break;

		case HIGHSHELF:
			return x;
            break;
	}
    return x;
}




