#pragma once

class reverb
{
	stereo* pBuffer;
	int nextSample;

public:
	reverb();
	~reverb();

	void init();

	stereo getnext(stereo sample);
};

