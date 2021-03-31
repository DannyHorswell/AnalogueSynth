#ifndef PATCH_H
#define PATCH_H

#include <string>
#include <vector>

using namespace std;

enum waveformtype
{
	MUTE	= 0,
	SQUARE 	= 1,
	SIN 	= 2,
	SAW 	= 3,
	NOISE 	= 4,
	RND_SQ	= 5,
	PCM		= 6
};

struct patchTDL // Time dependany level
{
	float T1;
	float L1;
	float T2;
	float L2;
	float T3;
	float Sustain;
	float T4;
	float L4;
};

enum WGMIX
{
	// Thes are useful for isolating single wave generators to test seperately from others in the voice
	WG0		 = 0,	// WG 0 only
	WG1		 = 1,	// WG 1 only
	WG2		 = 2,	// WG 2 only
	WG3		 = 3,	// WG 3 only
	WG4		 = 4,	// WG 4 only
	WG5		 = 5,	// WG 5 only
	WG6		 = 6,	// WG 6 only
	WG7		 = 7,	// WG 7 only

	MIX		 = 65,	 // Outputs are mixed equally left and right
	RING 	 = 66,   // WG pairs fed into ring modulator, outputs equal left and right
	RING_MIX = 67,   // WGs fed into ring modulator and mixed with first WG, outputs equal left and right
};

enum filtertype
{
	OFF			= 0,
	LPF			= 1,
	HPF			= 2,
	BPF		= 3,
	BPF2		= 4,
	NOTCH		= 5,
	APF			= 6,
	PEAKINGEQ	= 7,
	LOWSHELF	= 8,
	HIGHSHELF	= 9
};

// Filter
struct patchFilter
{
	filtertype FilterType;
	float Q;
	float RelativeFrequency; // Relative to note, freq
	float VelocityFactor; // 0 = Cutoff frequency doesn not change with velocity

	float DBGain; // Peak and shelf filters only
};

struct patchSampleSet
{
	int NSamples;
	string FileNames[MAX_SAMPLES_PER_SAMPLESET];
	float SampledNoteFrequency[MAX_SAMPLES_PER_SAMPLESET];
	int LoopStartSample[MAX_SAMPLES_PER_SAMPLESET];
	int LoopEndSample[MAX_SAMPLES_PER_SAMPLESET];
};

// Patch portion for wave generator
struct patchWG
{
	waveformtype _type;			// Waveform type
	float keyOffestSemitones; 	// Key offest in semitones
	bool enablePitchBend;		// If true, pitch bend changes pitch
	float pitchBendAmount;		// Amount of pitch bend scale
	
	float velocityVolumeAdjust; // 0 = No adjust 1.0 = full. -1.0 = full negative
	float velocityPanAdjust;    // 0 = No adjust 1.0 = full. -1.0 = full negative
	float fixedPanAdjustment;   // 0 = No adjust 1.0 = full. -1.0 = full negative
	float keyPanAdjustment;     // 0 = No adjust 1.0 = full. -1.0 = full negative

	// LFO Frequency change
	int freqLFOid;				// The ID of the pitch LFO
	float freqLFOLevel;			// Amount pitch is altered by LFO
	float freqLFODelay;			// Seconds till freq LFO starts

	// LFO PWM
	int pwmLFOid;				// The ID of the PWM LFO
	float pwmLFOLevel;			// Amount PWM is altered by LFO

	// Time dependants
#ifdef ENABLE_TDP
	patchTDL TDP;				// Time dependant pitch
#endif

#ifdef ENABLE_TDA
	patchTDL TDA;				// Time dependant ampliude
#endif

#ifdef ENABLE_TDF
	patchTDL TDF;				// Time dependant filter
#endif

	patchFilter Filter;			// The filter

	patchSampleSet SampleSet;	// The sampleset

	int sampleSetId;
};

struct patchLFO
{
	waveformtype _type;		// Waveform type
	float frequency; 		// Frequency of LFO
};

struct patchReverb
{
	bool enabled;
	int sampleLength;
	float feedback;
	float level;
};

// Whole patch for a voice
struct patch
{
	// Low frequency oscillators
	struct patchLFO LFOs[NUMBER_OF_LFO_FOR_SYNTH];

	// Wave generators
	struct patchWG WGs[NUMBER_OF_WAVE_GENERATORS_PER_VOICE];
	
	// Reverb settings
	struct patchReverb Reverb;

	// WGs are mixing
	WGMIX WGMixMode;
};

// Reads a patch file to the patch pointer
extern vector<string> split( const string& s, const string& f );
extern bool ReadPatchFromFile(patch* pPatch, char* FileName);
extern void PopulateDefaultPatch(patch* pPatch);
extern void SavePatch(synth*pSynth, patch* pPatch, char* filePath);
extern bool ProcessPatchLine(patch* pPatch, const string& s, bool& wasSampleSetChange);

#endif

