#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "noise.h"

noise theNoiseGenerator;

noise::noise()
{
	printf("noise ctor\n");

	pNoiseBuffer = (float*) malloc (NOISE_BUFFER_SIZE * sizeof(float));

	buffPos1 = 0;
	buffPos2 = 0;

	float val;

	for (int count=0; count<NOISE_BUFFER_SIZE; count++)
	{
		val = -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
		val += -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
		val += -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
		val += -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
		val += -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;
		val += -1.0F + ((float) rand() / (float) RAND_MAX) * 2.0F;

		val = val / 6.0F;

		pNoiseBuffer[count] = val;
	}
}

noise::~noise()
{
	free(pNoiseBuffer);
}



