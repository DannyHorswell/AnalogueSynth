#pragma once

#ifndef LCHFILTER_H
#define LCHFILTER_H

#include "patch.h"


class LCRFilter
{

private:
    synth* _pSynth;
    voice* _pVoice;
	wavegenerator* _pWG;
	int _wgID;

    float _deltaT;


    // Inductor, capacitor and resistor values
    float L = 1.0F;
    float C;
    float R;

    // Reverse laplace transform coeffiecnts
    float Alpha;
    float Beta;

    // Sin and cosine components
    float VzeroComponent;
    float IzeroComponent;

    // Initial conditions
    float Vc;
    float I;

    void RecalculateValues(float frequency, float q);
public:
    

    void init(synth* pSynth, voice* pVoice, int wgID, wavegenerator* pWG, float deltaT);

	void recalculateCoefficients();

    float getNext(float x);
};

#endif

