#include <stdio.h>
#include <string>
#include <vector>

#include "constants.h"
#include "patch.h"
#include "LCRFilter.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "voice.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"

const int TVL_RECALC_NUM = 256;

voice::voice()
{
	//fprintf(stderr, "voice ctor\n");
}

voice::~voice()
{

}

void voice::init(synth* pSynth, float deltaT)
{
	fprintf(stderr, "voice init\n");

	_pSynth = pSynth;

	for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
	{
		waveGenerators[count].init(pSynth, this, count, deltaT);

#ifdef ENABLE_TDP
		TDPs[count].init(pSynth, this, count, P);
#endif

#ifdef ENABLE_TDA
		TDAs[count].init(pSynth, this, count, A);
#endif

#ifdef ENABLE_TDF
		TDFs[count].init(pSynth, this, count, F);
#endif
	}
}

stereo voice::getnext(float deltaT)
{
#ifdef PRINT_GETNEXT
	printf("voice::getnext\n");
#endif

	if (keyPressed)
	{
		timeSincePressed += deltaT;
	}
	else
	{
		timeSinceReleased += deltaT;
	}

	// Recalculate TDLs
	if (tvlRecalcCount++ == 0)
	{
		for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
		{

#ifdef ENABLE_TDP
			TDPs[count].getnext();
#endif

#ifdef ENABLE_TDA
			TDAs[count].getnext();
#endif

#ifdef ENABLE_TDF
			TDFs[count].getnext();
#endif
		}
		
		tvlDeltaT = 0;
	}
	else
	{
		tvlDeltaT += deltaT;
	}

	if (tvlRecalcCount >= TVL_RECALC_NUM)
	{
		tvlRecalcCount = 0;
	}

	switch (_pSynth->_pSelectedPatch->WGMixMode)
	{
		// The following cases used to isolate single WGs for testing
		case WG0:
		case WG1:
		case WG2:
		case WG3:
		case WG4:
		case WG5:
		case WG6:
		case WG7:

			if (_pSynth->_pSelectedPatch->WGMixMode < NUMBER_OF_WAVE_GENERATORS_PER_VOICE)
			{
				output = waveGenerators[_pSynth->_pSelectedPatch->WGMixMode].getnext(deltaT);

				#ifdef ENABLE_TDA
				output.left = output.left * TDAs[_pSynth->_pSelectedPatch->WGMixMode].output;
				output.right = output.right * TDAs[_pSynth->_pSelectedPatch->WGMixMode].output;
				#endif
			}
			else
			{
				// Wave generator does not exist
				output.left = 0.0F;
				output.right = 0.0F;
			}
			break;

		case RING:
			
			{
				output.left = 0.0F;
				output.right = 0.0F;

				stereo last;
				bool lastUsed = true;

				for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
				{
					stereo next = waveGenerators[count].getnext(deltaT);

					#ifdef ENABLE_TDA
					next.left = next.left * TDAs[count].output;
					next.right = next.right * TDAs[count].output;
					#endif

					if (!(count & 1))
					{
						// Store the output to be multiplied with next
						last = next;
						lastUsed = false;
					}
					else
					{
						output.left += next.left * last.left;
						output.right += next.right * last.right;
						lastUsed = true;
					}
				}

				// If we have an odd number of wgs, the last one is added
				if (!lastUsed)
				{
					output.left += last.left;
					output.right += last.right;
				}

				output.left /= NUMBER_OF_WAVE_GENERATORS_PER_VOICE;
				output.right /= NUMBER_OF_WAVE_GENERATORS_PER_VOICE;
			}
			break;

		case RING_MIX:

			{
				output.left = 0.0F;
				output.right = 0.0F;

				stereo last;
				bool lastUsed = true;

				for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
				{
					stereo next = waveGenerators[count].getnext(deltaT);

					#ifdef ENABLE_TDA
					next.left = next.left * TDAs[count].output;
					next.right = next.right * TDAs[count].output;
					#endif

					if (!(count & 1))
					{
						// Store the output to be multiplied with next
						last = next;
						lastUsed = false;
					}
					else
					{
						output.left +=  (last.left + next.left * last.left) / 2.0F;
						output.right += (last.right + next.right * last.right) / 2.0F;
						lastUsed = true;
					}
				}

				// If we have an odd number of wgs, the last one is added
				if (!lastUsed)
				{
					output.left += last.left;
					output.right += last.right;
				}

				//output.left /= NUMBER_OF_WAVE_GENERATORS_PER_VOICE;
				//output.right /= NUMBER_OF_WAVE_GENERATORS_PER_VOICE;
			}
			break;
			
		case MIX:
		default:

			output.left = 0.0F;

			for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
			{
				#ifdef ENABLE_TDA
					output.left += waveGenerators[count].getnext(deltaT).left * TDAs[count].output;
					output.right += waveGenerators[count].getnext(deltaT).right * TDAs[count].output;
				#else
					output.left += waveGenerators[count].getnext(deltaT).left;
					output.right += waveGenerators[count].getnext(deltaT).right;
				#endif
			}
			

			output.left /= NUMBER_OF_WAVE_GENERATORS_PER_VOICE;
			output.right /= NUMBER_OF_WAVE_GENERATORS_PER_VOICE;
			break;
	}

	return output;
}

void voice::setkey(int midiKey, int midiVelocity)
{
	key = (float) midiKey;
	velocity = (float) midiVelocity;
	keyPressed = true;
	timeSincePressed = 0.0F;

	for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
	{
		waveGenerators[count].keyPress(velocity, key);
	}
}

void voice::releasekey()
{
	keyPressed = false;
	timeSinceReleased = 0.0F;
}


