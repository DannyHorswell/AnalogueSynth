#include "LCRFilter.h"
#include "constants.h"

#include <math.h>


/// <summary>
/// These need recalculating when f and q change
/// </summary>
/// <param name="frequency"></param>
/// <param name="q"></param>
void LCRFilter::RecalculateValues(float frequency, float q)
{
    C = 1 / (2 * PI * frequency);
    C = C * C;

    R = sqrt(1 / C) / q;

    Alpha = R / 2;
    Beta = sqrt((1 / C) + (R * R) / 4);
}

/// <summary>
/// These need calculating once when sample rate is known
/// </summary>
/// <param name="t"></param>
void LCRFilter::CalculateComponents(float t)
{
    float exponentBit = exp(-Alpha * t);
    float sinBit = sin(Beta * t);
    float cosBit = cos(Beta * t);

    VzeroComponent = (exponentBit * sinBit) / Beta;
    IzeroComponent = exponentBit * cosBit;
}


/// <summary>
/// Get teh next sample
/// </summary>
/// <param name="detlaT"></param>
/// <param name="inputV"></param>
/// <returns></returns>
float LCRFilter::NextSample(float detlaT, float inputV)
{
    float vdiff = inputV - Vc;

    float i = vdiff * VzeroComponent + I * IzeroComponent;

    // Store I for next iteration
    I = i;

    // Calculate diference in Vc
    float dv = (I * detlaT) / C;

    // Store Vc for next iteration
    Vc += dv;

    return Vc;
}
