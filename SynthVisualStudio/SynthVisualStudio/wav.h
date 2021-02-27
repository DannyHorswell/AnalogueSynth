#ifndef WAV_H
#define WAV_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include "constants.h"

using namespace std;

struct WAVHeader
{
	char ChunkId[4];
	uint32_t ChunkSize;
	char Format[4];
	char SubChunk1ID[4];
	uint32_t Subchunk1Size;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	char Subchunk2ID[4];
	uint32_t Subchunk2Size;
};

class wav
{
	float* pSampleBuffer;
	float sampledNotePeriod;

public:
	string fileName;

	int nSamples;
	float sampleRate;
	float samplePeriod;
	float sampleLengthTime;
	float sampledNoteFrequency;

	int _loopStartSample;
	int _loopEndSample;

	WAVHeader header;
	int _writeSamplePos;

	wav();
	~wav();


	// Initalises new wav of supplied n stereo samples
	void initnew(long nSamples);
	void addSample(stereo sample);
	void saveFile(const string& FileName);

	// If loopEndSample = 0, no loop
	void loadFile(const string& FileName, float noteFrequency, int loopStartSample, int loopEndSample);
	float getNext(float& positionTime, float deltaT, float _full_period, bool keyPressed);
};

#endif