
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

    float exponentBit = expf(-Alpha * _deltaT);
    float sinBit = sinf(Beta * _deltaT);
    float cosBit = cosf(Beta * _deltaT);

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

    vdiff = x - Vc;

    i = vdiff * VzeroComponent + I * IzeroComponent;

    // Store I for next iteration
    I = i;

    // Calculate diference in Vc
    dv = (I * _deltaT) / C;

    // Store Vc for next iteration
    Vc += dv;


    switch (pPatchFilter->FilterType)
	{
        case filtertype::OFF:
            return x;
            break;

        case filtertype::LPF:
        case filtertype::HIGHSHELF:
            return Vc;
            break;

        case filtertype::HPF:
        case filtertype::LOWSHELF:
            return x - Vc - (i * R);
            break;

        case filtertype::BPF:
        case filtertype::BPF2:
        case filtertype::PEAKINGEQ:
            return i * R;
            break;

        case filtertype::NOTCH: // Band stop filter
            return x - (i * R);
            break;

        case filtertype::APF:
            return x;
            break;
	}
    return x;
}




