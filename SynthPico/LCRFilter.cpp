
#include <math.h>

#include "signal.h"
#include "constants.h"
#include "patch.h"
#include "LCRFilter.h"



void LCRFilter::init(signal deltaT)
{
    _deltaT = deltaT;
}

/// <summary>
/// These need recalculating when f and q change
/// </summary>
/// <param name="frequency"></param>
/// <param name="q"></param>
void LCRFilter::RecalculateValues(signal frequency, signal q)
{
    C = sig_1 / (sig_TWO_PI * frequency);
    C = C * C;

    R = sqrtf( (sig_1 / (C * q)).ToFloat() );

    Alpha = R / sig_2;
    Beta = sqrtf(   ((sig_1 / C) + (R * R) / 4.0F ).ToFloat()   );

    signal exponentBit = expf( (sig_0-Alpha * _deltaT).ToFloat() );
    signal sinBit = sinf( (Beta * _deltaT).ToFloat() );
    signal cosBit = cosf( (Beta * _deltaT).ToFloat() );

    VzeroComponent = (exponentBit * sinBit) / Beta;
    IzeroComponent = exponentBit * cosBit;
}

#ifdef ENABLE_TDF
void LCRFilter::recalculateCoefficients(keyPressedInfo kpi, tld TDF)
{
    signal key = signal(kpi.key) + (signal(kpi.velocity) / sig_64) * GPatch.Filter.VelocityFactor;

    key += TDF.output;

    float freq = GSynth.KeyNumberToFrequency(key);

    freq *= GPatch.Filter.RelativeFrequency;

    if (freq > maxFreq)
    {
        freq = maxFreq;
    }

    signal Q =  GPatch.Filter.Q;

    RecalculateValues(freq, Q);
}
#endif







