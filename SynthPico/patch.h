#pragma once

#include <string>
#include <vector>

using namespace std;

enum class waveformtype
{
	MUTE	= 0,
	SQUARE 	= 1,
	SIN 	= 2,
	SAW 	= 3,
	NOISE 	= 4,
	RND_SQ	= 5
};

struct patchTDL // Time dependany level
{
	signal T1;
	signal L1;
	signal T2;
	signal L2;
	signal T3;
	signal Sustain;
	signal T4;
	signal L4;
};

enum class WGMIX
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

enum class filtertype
{
	OFF			= 0,
	LPF			= 1,
	HPF			= 2,
	BPF			= 3,
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
	signal Q;
	signal RelativeFrequency; // Relative to note, freq
	signal VelocityFactor; // 0 = Cutoff frequency doesn not change with velocity

	signal DBGain; // Peak and shelf filters only
};

// Patch portion for wave generator
struct patchWG
{
	waveformtype _type;			// Waveform type
	signal keyOffestSemitones; 	// Key offest in semitones
	bool enablePitchBend;		// If true, pitch bend changes pitch
	signal pitchBendAmount;		// Amount of pitch bend scale
	
	signal velocityVolumeAdjust; // 0 = No adjust 1.0 = full. -1.0 = full negative
	signal velocityPanAdjust;    // 0 = No adjust 1.0 = full. -1.0 = full negative
	signal fixedPanAdjustment;   // 0 = No adjust 1.0 = full. -1.0 = full negative
	signal keyPanAdjustment;     // 0 = No adjust 1.0 = full. -1.0 = full negative

	// LFO Frequency change
	int freqLFOid;				// The ID of the pitch LFO
	signal freqLFOLevel;			// Amount pitch is altered by LFO
	signal freqLFODelay;			// Seconds till freq LFO starts

	// LFO PWM
	int pwmLFOid;				// The ID of the PWM LFO
	signal pwmLFOLevel;			// Amount PWM is altered by LFO

	// Time dependants
#ifdef ENABLE_TDP
	patchTDL TDP;				// Time dependant pitch
#endif

#ifdef ENABLE_TDA
	patchTDL TDA;				// Time dependant ampliude
#endif

	//signal mixLevel = sig_1;
	//signal fmLevel = sig_0;
};

struct patchLFO
{
	waveformtype _type;		// Waveform type
	signal frequency; 		// Frequency of LFO
};

struct patchReverb
{
	bool enabled;
	int sampleLength;
	signal feedback;
	signal level;
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

	#ifdef ENABLE_TDF
		patchTDL TDF;				// Time dependant filter
	#endif

	patchFilter Filter;			// The filter

	// WGs are mixing
	WGMIX WGMixMode;
};

// Reads a patch file to the patch pointer
extern vector<string> split( const string& s, const string& f );
extern bool ReadPatchFromFile(char* FileName);
extern void PopulateDefaultPatch();
//extern void SavePatch(synth*pSynth, patch* pPatch, char* filePath);
extern bool ProcessPatchLine(const string& s);


