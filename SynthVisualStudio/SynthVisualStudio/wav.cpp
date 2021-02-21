#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "constants.h"
#include "wav.h"

wav::wav()
{
	pSampleBuffer = NULL;
}

wav::~wav()
{
	if (pSampleBuffer)
	{
		printf("Free pSampleBuffer\n");
		free(pSampleBuffer);
	}
}

struct WAVHeader
{
	char ChunkId[4];
	uint32_t ChunkSize;
	char Format[4];
	uint32_t SubChunk1ID;
	uint32_t Subchunk1Size;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	uint32_t Subchunk2ID;
};

void wav::loadFile(const string& FileName, float noteFrequency, int loopStartSample, int loopEndSample)
{
	// These parameters can be chnages without loading a file
	_loopStartSample = loopStartSample;
	_loopEndSample = loopEndSample;
	sampledNoteFrequency = noteFrequency;

	printf("Loop start %i, LoopEnd %i\n", _loopStartSample, _loopEndSample);

	if (fileName == FileName)
	{
		printf("File %s already loaded\n", FileName.c_str());
	}
	else
	{
		// File name has changed so free up existing and load new one

		// If we have a sample already loaded, free it
		if (pSampleBuffer)
		{
			printf("Free pSampleBuffer\n");
			free(pSampleBuffer);
		}

		fprintf(stderr, "Load sample file %s\n", FileName.c_str());

		fileName = FileName;
	
		sampledNotePeriod = sampledNoteFrequency;

		ifstream ifs(FileName.c_str(), ios::binary);

		FILE* csvFile = fopen ("wav.csv","w");

		char readData[44];
		int headerChunkSize;

		int dataLength;

		float maxValue = 0.0;

		if (ifs)
		{
			// get length of file:
			ifs.seekg (0, ios::end);
			dataLength = ifs.tellg();
			ifs.seekg (0, ios::beg);

			dataLength -= sizeof(WAVHeader);

			// Read the header
			ifs.read(readData, sizeof(WAVHeader));
		
			WAVHeader* pheader = (WAVHeader*) readData;

			fprintf(stderr, "ChunkId %c%c%c%c\n", pheader->ChunkId[0],pheader->ChunkId[1],pheader->ChunkId[2],pheader->ChunkId[3]);
			fprintf(stderr, "Format %c%c%c%c\n", pheader->Format[0],pheader->Format[1],pheader->Format[2],pheader->Format[3]);
			fprintf(stderr, "Audio Format %p\n", pheader->AudioFormat);
			fprintf(stderr, "Sample Rate %p\n", pheader->SampleRate);
			fprintf(stderr, "Num Channels %u\n", pheader->NumChannels);
			fprintf(stderr, "Bits Per Sample %u\n", pheader->BitsPerSample);
			fprintf(stderr, "DataLength %u\n", dataLength);

			sampleRate = (float) pheader->SampleRate;

			samplePeriod = 1.0F / sampleRate;

			int bytesPerSample = ((int) pheader->BitsPerSample) / 8;

			nSamples = dataLength / bytesPerSample / pheader->NumChannels;

			sampleLengthTime = nSamples / sampleRate;

			pSampleBuffer = (float*) malloc (nSamples * sizeof(float));

			char pByte[1];
			char ptheShortBytes[2];
			char ptheIntBytes[4];

			ptheIntBytes[0] = 0;
			ptheIntBytes[1] = 0;
			ptheIntBytes[2] = 0;
			ptheIntBytes[3] = 0;

			float theFloat;

			float valScale;

			// 8 bit sample
			if (bytesPerSample == 1)
			{
				valScale = (float) (128 * pheader->NumChannels);
			}

			// 16 bit sample
			if (bytesPerSample == 2)
			{
				valScale = (float) (FLOAT_TO_SIGNED_16_MULTIPLIER * pheader->NumChannels);
			}

			// 24 bit sample
			if (bytesPerSample == 3)
			{
				valScale = (float) (FLOAT_TO_SIGNED_24_MULTIPLIER * pheader->NumChannels);
			}

			// 32 bit float sample
			if (bytesPerSample == 4)
			{
			
			}

			for (int count=0; count<nSamples; count++)
			{
				float val = 0;

				// Mix multi channel
				for (int count2=0; count2<pheader->NumChannels; count2++)
				{
					if (bytesPerSample == 1)
					{
						ifs.read(pByte, 1);

						int ival = (int) (unsigned char) pByte[0];
						val += (float) ival;
					}

					if (bytesPerSample == 2)
					{
						ifs.read(ptheShortBytes, 2);

						signed short theInt = * ((signed short*) ptheShortBytes);

						val += (float) theInt;
					}

					if (bytesPerSample == 3)
					{
						ifs.read(ptheIntBytes, 3);

						if (csvFile)
						{
							fprintf(csvFile, "%i, %i, %i\n", (int) ptheIntBytes[0], (int) ptheIntBytes[1], (int) ptheIntBytes[2]);
						}

						signed int theInt;

						/*if (ptheIntBytes[0] & 0x80)
						{
							theInt = 0xFF000000 | (ptheIntBytes[0] << 16); // || (theIntBytes[1] << 8); // | ptheIntBytes[0];
						}
						else
						{
							theInt = (ptheIntBytes[0] << 16);// | (ptheIntBytes[1] << 8);// | ptheIntBytes[0];
						}*/

						theInt = ((signed int) (ptheIntBytes[0])) * 65536 +
								((signed int) (ptheIntBytes[1])) * 256;
								((signed int) (ptheIntBytes[2]));

						val += (float) theInt;
					}

					if (bytesPerSample == 4)
					{
						ifs.read( (char*) &theFloat, 4);

						val += theFloat;
					}
				}

				if (bytesPerSample == 1)
				{
					val = (val - valScale);  // (float) (128 * pheader->NumChannels));
					val = val / valScale;
				}

				if (bytesPerSample == 2)
				{
					val = val / valScale;
				}

				if (bytesPerSample == 3)
				{
					val = val / valScale;
				}

	#ifdef NORMALSE_SAMPLES
				if (val > maxValue)
				{
					maxValue = val;
				}

				if (val < -maxValue)
				{
					maxValue = -val;
				}
	#endif
				//fprintf(stderr, "Val %f\n", val);

				pSampleBuffer[count] = val;
			}

			ifs.close();

			fclose(csvFile);

	#ifdef NORMALSE_SAMPLES
			// Normalise
			for (int count=0; count<nSamples; count++)
			{
				pSampleBuffer[count] /= maxValue;
			}
	#endif
		}
	}
}


float wav::getNext(float& positionTime, float deltaT, float _full_period, bool keyPressed)
{
	float positionSampleNumberMultiplier = (float) nSamples / sampleLengthTime;

	int sampleNumber = (int) (positionTime * positionSampleNumberMultiplier);

	if (_loopEndSample > 0 && sampleNumber > _loopEndSample)
	{
		int loopLength = _loopEndSample - _loopStartSample;

		sampleNumber -= loopLength;
		positionTime -= loopLength / positionSampleNumberMultiplier;
	}

	// Increase position for next 
	positionTime += (deltaT / (INITIAL_MASTER_TUNE * _full_period)) * (INITIAL_MASTER_TUNE / sampledNoteFrequency);

	if (sampleNumber < nSamples)
	{
		return pSampleBuffer[sampleNumber];
	}
	else
	{
		
		// Return a little bit of noise as just 0.0 mucks up the biquad filter
		return -0.001F + ((float) rand() / (float) RAND_MAX) * 0.002F;
	}
}