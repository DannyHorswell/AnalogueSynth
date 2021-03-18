#ifndef TDL_H
#define TDL_H

enum tdlType
{
	P = 0,
	A = 1,
	F = 2
};

class tdl
{
	tdlType _type;
	synth* _pSynth;
	voice* _pVoice;
	int _wgID; 

public:
	float output;
	float releaseLevel;
	float pressLevel;
	bool lastKeystate;

	tdl();
	~tdl();

	void init(synth* pSynth, voice* pVoice, int wgID, tdlType type);

	float getnext();
};

#endif
