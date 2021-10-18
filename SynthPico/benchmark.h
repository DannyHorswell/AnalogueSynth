#pragma once

using namespace std;

class benchmark
{
	synth* pTheSynth;

	float deltaT;

	time_t start;
	time_t stop;
	time_t result;

	int nOps;

public:
	benchmark();
	~benchmark();

	float testsynth();
	float testfilterspeed();
	float testfiltercoefficentcalculation();
	float testwavegenerator();
	void run();
};
