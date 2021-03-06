#pragma once
class LCRFilter
{

private:
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

    void CalculateComponents(float t);

public:
    void RecalculateValues(float frequency, float q);

    float NextSample(float detlaT, float inputV);
};

