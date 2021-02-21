
#ifndef NOISE_H
#define NOISE_H

// Noise buffer
#define NOISE_BUFFER_SIZE 65536

class noise
{
	float* pNoiseBuffer;
	int buffPos1;
	int buffPos2;

public:

	noise();
	~noise();
	
	inline float getnext()
	{
		// Noise is combined from 2 changing positions in the noise buffer
		// one backward, one forward
		float ret = pNoiseBuffer[buffPos1++] + pNoiseBuffer[buffPos2];

		buffPos2--;
		buffPos2--;

		if (buffPos1 >= NOISE_BUFFER_SIZE)
		{
			buffPos1 = 0;
		}

		if (buffPos2 < 0)
		{
			buffPos2 = NOISE_BUFFER_SIZE - 1;
		}

		return ret;
	}

};

extern noise theNoiseGenerator;


#endif