
#ifndef BENCHMARK_H
#define BENCHMARK_H

using namespace std;


class benchmark
{
	synth theSynth;

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

#endif