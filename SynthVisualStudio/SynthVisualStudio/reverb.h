#ifndef REVERB_H
#define REVERB_H

class reverb
{
	synth* _pSynth;

	stereo* pBuffer;
	int nextSample;

public:
	reverb();
	~reverb();

	void init(synth* pSynth);

	stereo getnext(stereo sample);
};

#endif