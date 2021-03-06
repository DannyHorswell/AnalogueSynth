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
    double L = 1.0F;
    double C;
    double R;

    // Reverse laplace transform coeffiecnts
    double Alpha;
    double Beta;

    // Sin and cosine components
    double VzeroComponent;
    double IzeroComponent;

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

