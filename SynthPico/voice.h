#pragma once


const int TVL_RECALC_NUM = 1024;

class voice
{
public:
	wavegenerator waveGenerators[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];

	#ifdef ENABLE_TDP
	tdl TDPs[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];
	#endif

	#ifdef ENABLE_TDA
	tdl TDAs[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];
	#endif

	#ifdef ENABLE_TDF
	tdl TDF;
	#endif

	signal sample;
	struct stereo output;

	keyPressedInfo _keyPressedInfo;

	int tvlRecalcCount;

	LCRFilter theFilter;

	voice();
	~voice();

	void init(signal deltaT);

	inline stereo getnext(float deltaT)
{

#ifdef PRINT_GETNEXT
	printf("voice::getnext\n");
#endif

	if (_keyPressedInfo.keyPressed)
	{
		_keyPressedInfo.timeSincePressed += deltaT;
	}
	else
	{
		_keyPressedInfo.timeSinceReleased += deltaT;
	}

	// Recalculate TDLs
	if (tvlRecalcCount++ >= TVL_RECALC_NUM)
	{
		for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
		{
			#ifdef ENABLE_TDP
			TDPs[count].getnext(_keyPressedInfo);
			#endif

			#ifdef ENABLE_TDA
			TDAs[count].getnext(_keyPressedInfo);
			#endif
		}

		#ifdef ENABLE_TDF
		TDF.getnext();
		#endif

		#ifdef ENABLE_FILTERS
		theFilter.recalculateCoefficients(); // In case things have changed
		#endif

		tvlRecalcCount = 0;
	}


	switch (GPatch.WGMixMode)
	{
		// The following cases used to isolate single WGs for testing
		case WGMIX::WG0:
		case WGMIX::WG1:
		case WGMIX::WG2:
		case WGMIX::WG3:
		case WGMIX::WG4:
		case WGMIX::WG5:
		case WGMIX::WG6:
		case WGMIX::WG7:

			if ((int) GPatch.WGMixMode < NUMBER_OF_WAVE_GENERATORS_PER_VOICE)
			{
				sample = waveGenerators[(int) GPatch.WGMixMode].getnext(deltaT, _keyPressedInfo, TDPs[(int) GPatch.WGMixMode]);

				output.left = waveGenerators[(int)GPatch.WGMixMode].panMultipliers.left;
				output.right = waveGenerators[(int)GPatch.WGMixMode].panMultipliers.left;

				#ifdef ENABLE_TDA
				output.left = output.left * TDAs[(int) GPatch.WGMixMode].output;
				output.right = output.right * TDAs[(int) GPatch.WGMixMode].output;
				#endif
			}
			else
			{
				// Wave generator does not exist
				output.left = 0.0F;
				output.right = 0.0F;
			}
			break;

		case WGMIX::RING:
			
			{
				output.left = 0.0F;
				output.right = 0.0F;

				stereo last;
				bool lastUsed = true;

				for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
				{
					sample = waveGenerators[count].getnext(deltaT, _keyPressedInfo, TDPs[count]);

					stereo next;

					next.left = waveGenerators[count].panMultipliers.left * sample;
					next.right = waveGenerators[count].panMultipliers.right * sample;

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

				output.left /= DIVEDER_PER_WG;
				output.right /= DIVEDER_PER_WG;
			}
			break;

		case WGMIX::RING_MIX:

			{
				output.left = 0.0F;
				output.right = 0.0F;

				stereo last;
				bool lastUsed = true;

				for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
				{
					sample = waveGenerators[count].getnext(deltaT, _keyPressedInfo, TDPs[count]);

					stereo next;

					next.left = waveGenerators[count].panMultipliers.left;
					next.right = waveGenerators[count].panMultipliers.right;

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
			
		case WGMIX::MIX:
		default:

			output.left = 0.0F;

			stereo last;

			for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
			{
				#ifdef ENABLE_TDA
					sample = waveGenerators[count].getnext(deltaT, _keyPressedInfo, TDPs[count]) * TDAs[count].output;
				#else
					sample = waveGenerators[count].getnext(deltaT);
				#endif

				//sample *= GPatch.WGs[count].mixLevel;

				output.left += sample;// * waveGenerators[count].panMultipliers.left;
				output.right += sample;//* waveGenerators[count].panMultipliers.right;

				#ifdef ENABLE_FILTERS
					output = theFilter.getNext(output); // Apply the filter
				#endif
			}
			
			output.left /= DIVEDER_PER_WG;
			output.right /= DIVEDER_PER_WG;
			break;
	}

	return output;
}

	void setkey(int midiKey, int midiVelocity);
	void releasekey();
};

