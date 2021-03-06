#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

NOT USED - THIS IS FOR A BIQUAD FILTER - THIS WAS UNSTABLE

// Biquad filter (see http://en.wikipedia.org/wiki/Digital_biquad_filter)
// Maths here (http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt)
	

filter::filter()
{

}

filter::~filter()
{

}

void filter::init(synth* pSynth, int wgID, wavegenerator* pWG)
{
	_pSynth = pSynth;
	_wgID = wgID;
	_pWG = pWG;
}

void filter::recalculateCoefficients()
{
	patchFilter* pPatchFilter = &_pSynth->_pSelectedPatch->WGs[_wgID].Filter;

	float key = _pWG->_pVoice->key + ((_pWG->_pVoice->velocity) / 64.0F) * pPatchFilter->VelocityFactor;

	if (key == lastKey &&
		pPatchFilter->Q == lastQ &&
		pPatchFilter->FilterType == lastType)
	{
		return; // Don't recalculate
	}


	/*
		Fs (the sampling frequency)

	f0 ("wherever it's happenin', man."  Center Frequency or
		Corner Frequency, or shelf midpoint frequency, depending
		on which filter type.  The "significant frequency".)

	dBgain (used only for peaking and shelving filters)

	Q (the EE kind of definition, except for peakingEQ in which A*Q is
		the classic EE Q.  That adjustment in definition was made so that
		a boost of N dB followed by a cut of N dB for identical Q and
		f0/Fs results in a precisely flat unity gain filter or "wire".)

		_or_ BW, the bandwidth in octaves (between -3 dB frequencies for BPF
		and notch or between midpoint (dBgain/2) gain frequencies for
		peaking EQ)

		_or_ S, a "shelf slope" parameter (for shelving EQ only).  When S = 1,
		the shelf slope is as steep as it can be and remain monotonically
		increasing or decreasing gain with frequency.  The shelf slope, in
		dB/octave, remains proportional to S for all other values for a
		fixed f0/Fs and dBgain.
		*/


	float freq = _pWG->_pVoice->_pSynth->KeyNumberToFrequency(key) * pPatchFilter->RelativeFrequency;
	float Q =  pPatchFilter->Q;

	//printf("freq = %f, Q =  %f\n", freq, Q);

	lastKey = key;
	lastQ = pPatchFilter->Q ;
	lastType = pPatchFilter->FilterType;

	/*Then compute a few intermediate variables:

	A  = sqrt( 10^(dBgain/20) )
		=       10^(dBgain/40)     (for peaking and shelving EQ filters only)

	w0 = 2*pi*f0/Fs

	cos(w0)
	sin(w0)

	alpha = sin(w0)/(2*Q)                                       (case: Q)
			= sin(w0)*sinh( ln(2)/2 * BW * w0/sin(w0) )           (case: BW)
			= sin(w0)/2 * sqrt( (A + 1/A)*(1/S - 1) + 2 )         (case: S)

		FYI: The relationship between bandwidth and Q is
				1/Q = 2*sinh(ln(2)/2*BW*w0/sin(w0))     (digital filter w BLT)
		or   1/Q = 2*sinh(ln(2)/2*BW)             (analog filter prototype)

		The relationship between shelf slope and Q is
				1/Q = sqrt((A + 1/A)*(1/S - 1) + 2)

	2*sqrt(A)*alpha  =  sin(w0) * sqrt( (A^2 + 1)*(1/S - 1) + 2*A )
		is a handy intermediate variable for shelving EQ filters.

		*/

	float w0 = TWO_PI*freq/ SAMPLE_FREQUENCY;

	float sin_w0 = sin(w0);

	float alpha = sin_w0/(2*Q);
	float cos_w0 = cos(w0);

	/* Finally, compute the coefficients for whichever filter type you want:
	(The analog prototypes, H(s), are shown for each filter
		type for normalized frequency.) */

	switch (pPatchFilter->FilterType)
	{
		case OFF:
				b0 = 1.0F;
				b1 = 0.0F;
				b2 = 0.0F;
				a0 = 1.0F;
				a1 = 0.0F;
				a2 = 0.0F;
			break;

		case LPF:
			{
				b1 = 1.0F - cos_w0;
				b0 = b1 /2;
				b2 = b0;
				a0 = 1.0F + alpha;
				a1 = -2.0F * cos_w0;
				a2 = 1.0F - alpha;
			}
			break;

		case HPF:
			{
				b0 = (1 + cos_w0) / 2.0F;
				b1 = -(1.0F + cos_w0);
				b2 = b0;
				a0 = 1.0F + alpha;
				a1 = -2.0F * cos_w0;
				a2 = 1.0F - alpha;
			}
			break;

		case BPF1:
			{
				b0 = Q * alpha;
				b1 = 0;
				b2 = -Q * alpha;
				a0 = 1.0F + alpha;
				a1 = -2 * cos_w0;
				a2 = 1.0F - alpha;
			}
			break;
			
		case BPF2:
			{
				b0 = alpha;
				b1 = 0.0F;
				b2 = -alpha;
				a0 = 1.0F + alpha;
				a1 = -2.0F * cos_w0;
				a2 = 1.0F - alpha;
			}
			break;

		case NOTCH:
			{
				b0 = 1.0F;
				b1 = -2.0F * cos_w0;
				b2 = 1.0F;
				a0 = 1.0F + alpha;
				a1 = b1;
				a2 = 1.0F - alpha;
			}
			break;

		case APF:
			{
				b0 = 1.0F - alpha;
				b1 = -2.0F * cos_w0;
				b2 = 1.0F + alpha;
				a0 = b2;
				a1 = b1;
				a2 = b0;
			}
			break;

		case PEAKINGEQ:
			{
				float A = sqrt(pow(10.0,(pPatchFilter->DBGain/20.0)));

				b0 = 1.0F + alpha * A;
				b1 = -2.0F * cos_w0;
				b2 = 1.0F - alpha * A;
				a0 = 1.0F + alpha / A;
				a1 = -2.0F *cos_w0;
				a2 = 1.0F - alpha / A;
			}
			break;

		case LOWSHELF:
			{
				float A = pow(10.0,(pPatchFilter->DBGain/40.0));
				float _2_sqrt_a_alpha = 2.0F*sqrt(A)*alpha;
				float a_plus_1 = A+1.0F;
				float a_minus_1 = A-1.0F;
				float a_plus_1C = a_plus_1 * cos_w0;
				float a_minus_1C = a_minus_1 * cos_w0;

				b0 =      A*(a_plus_1 - a_minus_1C + _2_sqrt_a_alpha);
				b1 = 2.0F*A*(a_minus_1 - a_plus_1C);
				b2 =      A*(a_plus_1 - a_minus_1C - _2_sqrt_a_alpha);
				a0 =         a_plus_1 + a_minus_1C + _2_sqrt_a_alpha;
				a1 =  -2.0F*(a_minus_1 + a_plus_1C);
				a2 =         a_plus_1 + a_minus_1C - _2_sqrt_a_alpha;
			}
			break;

		case HIGHSHELF:
			{
				float A = pow(10.0, (pPatchFilter->DBGain/40.0));
				float _2_sqrt_a_alpha = 2.0F*sqrt(A)*alpha;
				float a_plus_1 = A+1.0F;
				float a_minus_1 = A-1.0F;
				float a_plus_1C = a_plus_1 * cos_w0;
				float a_minus_1C = a_minus_1 * cos_w0;

				b0 =       A*( a_plus_1 + a_minus_1C + _2_sqrt_a_alpha);
				b1 = -2.0F*A*( a_minus_1 + a_plus_1C);
				b2 =       A*( a_plus_1 + a_minus_1C - _2_sqrt_a_alpha);
				a0 =           a_plus_1 - a_minus_1C + _2_sqrt_a_alpha;
				a1 =    2.0F*( a_minus_1 - a_plus_1C);
				a2 =           a_plus_1 - a_minus_1C - _2_sqrt_a_alpha;
			}
			break;
	}

	// Normalise to make a[0] = 1
	b0 /= a0;
	b1 /= a0;
	b2 /= a0;
	a1 /= a0;
	a2 /= a0;
	//a0 /= a0 // don't bother

	//printf("%f, %f, %f, %f, %f\n", b0, b1, b2, a1, a2); // print coefficients
}




