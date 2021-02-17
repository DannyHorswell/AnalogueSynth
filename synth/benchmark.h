
#ifndef BENCHMARK_H
#define BENCHMARK_H


class benchmark
{
	synth theSynth;

	float deltaT;

	struct timeval start;
	struct timeval stop;
	struct timeval result;

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