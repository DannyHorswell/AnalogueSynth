#pragma once

#include "constants.h"

class pan
{
public:
	pan() {};
	~pan() {};

	inline stereo GetStereoMultipliers(float panValue)
	{
		stereo ret;

		panValue < -1.0F ? -1.0F : panValue;
		panValue > 1.0F ? 1.0F : panValue;

		panValue = panValue + 1.0F; // Moves range from 0 to + 2
		panValue = panValue * QUATER_PI; //Moves range to 0 to PI/2

		ret.left = wg_sin(panValue);
		ret.right = wg_sin(HALF_PI - panValue);

		return ret;
	}
};

