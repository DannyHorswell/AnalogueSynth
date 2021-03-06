
#ifndef FILTER_H
#define FILTER_H

#include "patch.h"


NOT USED - THIS IS FOR A BIQUAD FILTER - THIS WAS UNSTABLE

const float FILTER_PEAK_THRESHOLD = 20.0F;
const float FILTER_NEG_PEAK_THRESHOLD = -20.0F;

class filter
{
	synth* _pSynth;
	wavegenerator* _pWG;
	int _wgID;

	// Direct form filter
	float b0;
	float b1;
	float b2;

	float a0;
	float a1;
	float a2;

	// Input buffer
	float x1;
	float x2;

	// Output buffer
	float y1;
	float y2;

	float lastKey;
	float lastQ;
	filtertype lastType;

public:
	float y;

	filter();
	~filter();

	void init(synth* pSynth, int wgID, wavegenerator* pWG);

	void recalculateCoefficients();
	{
		// y[n] = (b0/a0)*x[n] + (b1/a0)*x[n-1] + (b2/a0)*x[n-2]
		//                    - (a1/a0)*y[n-1] - (a2/a0)*y[n-2]

		// Note we have normalised a0 to be 1
        y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;

        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;

		if (y > FILTER_PEAK_THRESHOLD)
		{
			y = FILTER_PEAK_THRESHOLD;
		}

		if (y < FILTER_NEG_PEAK_THRESHOLD)
		{
			y = FILTER_NEG_PEAK_THRESHOLD;
		}

		// If things have really an awary, reset to 0
		if (y != y) // y is Nan
		{
			y = x;
			y1 = 0.0F;
			y2 = 0.0F;
			x1 = 0.0F;
			x2 = 0.0F;
		}

        return (float) y;
	}
};

#endif