#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <math.h>

#include "constants.h"
#include "patch.h"
#include "sampleset.h"


vector<string> split( const string& s, const string& f )
{
    vector<string> temp;
    if ( f.empty() ) {
        temp.push_back( s );
        return temp;
    }
    typedef string::const_iterator iter;
    const iter::difference_type f_size( distance( f.begin(), f.end() ) );
    iter i( s.begin() );
    for ( iter pos; ( pos = search( i , s.end(), f.begin(), f.end() ) ) != s.end(); ) {
        temp.push_back( string( i, pos ) );
        advance( pos, f_size );
        i = pos;
    }
    temp.push_back( string( i, s.end() ) );
    return temp;
}

string removechars(const string& str, const string& chars)
{
	string ret;

	for (int count=0; count<str.size(); count++)
	{
		bool ok = true;

		for (int count2=0; count2<chars.size(); count2++)
		{
			char a = str[count];
			char b = chars[count2];

			if (a == b)
			{
				ok = false;
			}
		}

		if (ok)
		{
			ret = ret + str[count];
		}
	}

	return ret;
}

struct NameIndexer
{
	string name;
	int indexer;
};

bool ProcessTDL(patchTDL* pTDL, vector<NameIndexer> nixers, string value)
{
	if (nixers[2].name == "L0")
	{
		pTDL->L0 = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "L1")
	{
		pTDL->L1 = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "L2")
	{
		pTDL->L2 = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "Sustain")
	{
		pTDL->Sustain = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "L4")
	{
		pTDL->L4 = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "T1")
	{
		pTDL->T1 = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "T2")
	{
		pTDL->T1 = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "T3")
	{
		pTDL->T1 = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "T4")
	{
		pTDL->T1 = atof(value.c_str());
		return true;
	}

	fprintf(stderr, "Unrecognised TD(A/F) setting : '%s'\n", nixers[2].name.c_str());
	return false;
}

bool ProcessSampleSet(patchSampleSet* sPatchSampleSet, int WGIndex, vector<NameIndexer> nixers, string value)
{
	if (nixers[2].name == "NSamples")
	{
		int nSamples = atoi(value.c_str());

		if (nSamples > MAX_SAMPLES_PER_SAMPLESET)
		{
			nSamples = MAX_SAMPLES_PER_SAMPLESET;
		}

		sPatchSampleSet->NSamples = nSamples;
		return true;
	}

	if (nixers[2].name == "Sample")
	{
		int index = nixers[2].indexer;

		if (nixers[3].name == "File")
		{
			sPatchSampleSet->FileNames[index] = value.c_str();
			return true;
		}

		if (nixers[3].name == "SampledNoteFrequency")
		{
			sPatchSampleSet->SampledNoteFrequency[index] = atof(value.c_str());
			return true;
		}

		if (nixers[3].name == "SampledMIDIKeyNumber")
		{
			int keyNumber = atoi(value.c_str());

			float freq = pow(2.0F, keyNumber / 12.0F) * 440 / pow(2.0F, (float) MIDDLE_A_KEY_NUMBER / 12.0F);

			printf("key = %i, freq= %f\n", keyNumber, freq);

			sPatchSampleSet->SampledNoteFrequency[index] = freq;
			return true;
		}

		if (nixers[3].name == "LoopStartSample")
		{
			sPatchSampleSet->LoopStartSample[index] = atoi(value.c_str());
			return true;
		}

		if (nixers[3].name == "LoopEndSample")
		{
			sPatchSampleSet->LoopEndSample[index] = atoi(value.c_str());
			return true;
		}

		fprintf(stderr, "Unrecognised Sample setting : '%s'\n", nixers[3].name.c_str());
		return false;
	}

	fprintf(stderr, "Unrecognised SampleSet setting : '%s'\n", nixers[2].name.c_str());
	return false;
}

bool ProcessFilter(patchFilter* pFilter, vector<NameIndexer> nixers, string value)
{
	if (nixers[2].name == "FilterType")
	{
		if (value == "OFF")
		{
			pFilter->FilterType=OFF;
			return true;
		}

		if (value == "LPF")
		{
			pFilter->FilterType=LPF;
			return true;
		}

		if (value == "HPF")
		{
			pFilter->FilterType=HPF;
			return true;
		}

		if (value == "BPF1")
		{
			pFilter->FilterType=BPF1;
			return true;
		}

		if (value == "BPF2")
		{
			pFilter->FilterType=BPF2;
			return true;
		}

		if (value == "NOTCH")
		{
			pFilter->FilterType=NOTCH;
			return true;
		}

		if (value == "APF")
		{
			pFilter->FilterType=APF;
			return true;
		}

		if (value == "PEAKINGEQ")
		{
			pFilter->FilterType=PEAKINGEQ;
			return true;
		}

		if (value == "LOWSHELF")
		{
			pFilter->FilterType=LOWSHELF;
			return true;
		}

		fprintf(stderr, "\nUnrecognised FilterType : '%s'\n\n", value.c_str());
		return false;
	}

	if (nixers[2].name == "Q")
	{
		pFilter->Q = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "RelativeFrequency")
	{
		pFilter->RelativeFrequency = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "DBGain")
	{
		pFilter->DBGain = atof(value.c_str());
		return true;
	}

	if (nixers[2].name == "VelocityFactor")
	{
		pFilter->VelocityFactor = atof(value.c_str());
		return true;
	}

	fprintf(stderr, "Unrecognised filter setting : '%s'\n", nixers[2].name.c_str());
	return false;
}

bool wgLimitWarning = false; // so we only print warning once

bool ProcessWG(patch* pPatch, int WGIndex, vector<NameIndexer> nixers, string value, bool& wasSampleSetChange)
{
	if (WGIndex >= NUMBER_OF_WAVE_GENERATORS_PER_VOICE && !wgLimitWarning)
	{
		fprintf(stderr, "Only %i wave generators supported in this build, requested WG[%i]\n", NUMBER_OF_WAVE_GENERATORS_PER_VOICE, WGIndex);
		wgLimitWarning = true;
		return false;
	}

	patchWG* pWG = &pPatch->WGs[WGIndex];

	if (nixers[1].name == "Type")
	{
		if (value == "MUTE")
		{
			pWG->_type = MUTE;
			return true;
		}

		if (value == "SQUARE")
		{
			pWG->_type = SQUARE;
			return true;
		}

		if (value == "SIN")
		{
			pWG->_type = SIN;
			return true;
		}

		if (value == "SAW")
		{
			pWG->_type = SAW;
			return true;
		}

		if (value == "NOISE")
		{
			pWG->_type = NOISE;
			return true;
		}

		if (value == "RND_SQ")
		{
			pWG->_type = RND_SQ;
			return true;
		}

		if (value == "PCM")
		{
			pWG->_type = PCM;
			return true;
		}
		
		fprintf(stderr, "\nUnrecognised WG type : '%s'\n\n", value.c_str());
		return false;
	}

	if (nixers[1].name == "EnablePitchBend")
	{
		if (value == "true")
		{
			pWG->enablePitchBend = true;
			return true;
		}

		if (value == "false")
		{
			pWG->enablePitchBend = true;
			return true;
		}

		fprintf(stderr, "Unrecognised EnablePitchBend value : '%s'", value.c_str());
		return false;
	}

	if (nixers[1].name == "PitchBendAmount")
	{
		pWG->pitchBendAmount = atof(value.c_str());
		return true;
	}

	if (nixers[1].name == "VelocityVolumeAdjust")
	{
		pWG->velocityVolumeAdjust = atof(value.c_str());
		return true;
	}
	
	if (nixers[1].name == "KeyOffestSemitones")
	{
		pWG->keyOffestSemitones = atof(value.c_str());
		return true;
	}

	if (nixers[1].name == "FreqLFOid")
	{
		pWG->freqLFOid = atoi(value.c_str());
		return true;
	}

	if (nixers[1].name == "FreqLFOLevel")
	{
		pWG->freqLFOLevel = atof(value.c_str());
		return true;
	}

	if (nixers[1].name == "FreqLFODelay")
	{
		pWG->freqLFODelay = atof(value.c_str());
		return true;
	}

	if (nixers[1].name == "PWMLFOid")
	{
		pWG->pwmLFOid = atoi(value.c_str());
		return true;
	}

	if (nixers[1].name == "PWMLFOLevel")
	{
		pWG->pwmLFOLevel = atof(value.c_str());
		return true;
	}

#ifdef ENABLE_TDP
	if (nixers[1].name == "TDP")
	{
		return ProcessTDL(&pWG->TDP, nixers, value);
	}
#endif

#ifdef ENABLE_TDA
	if (nixers[1].name == "TDA")
	{
		return ProcessTDL(&pWG->TDA, nixers, value);
	}
#endif

#ifdef ENABLE_TDF
	if (nixers[1].name == "TDF")
	{
		return ProcessTDL(&pWG->TDF, nixers, value);
	}
#endif

	if (nixers[1].name == "Filter")
	{
		return ProcessFilter(&pWG->Filter, nixers, value);
	}

	if (nixers[1].name == "SampleSet")
	{
		wasSampleSetChange = ProcessSampleSet(&pWG->SampleSet, WGIndex, nixers, value);
		return wasSampleSetChange;
	}

	fprintf(stderr, "Unrecognised WG setting : '%s'\n", nixers[1].name.c_str());
	return false;
}

bool ProcessLFO(patch* pPatch, int LFOIndex, vector<NameIndexer> nixers, string value)
{
	if (nixers[1].name == "Frequency")
	{
		pPatch->LFOs[LFOIndex].frequency = atof(value.c_str());
		return true;
	}

	if (nixers[1].name == "Type")
	{
		if (value == "MUTE")
		{
			pPatch->LFOs[LFOIndex]._type = MUTE;
			return true;
		}

		if (value == "SQUARE")
		{
			pPatch->LFOs[LFOIndex]._type = SQUARE;
			return true;
		}

		if (value == "SIN")
		{
			pPatch->LFOs[LFOIndex]._type = SIN;
			return true;
		}

		if (value == "SAW")
		{
			pPatch->LFOs[LFOIndex]._type = SAW;
			return true;
		}

		if (value == "NOISE")
		{
			pPatch->LFOs[LFOIndex]._type = NOISE;
			return true;
		}

		if (value == "RND_SQ")
		{
			pPatch->LFOs[LFOIndex]._type = RND_SQ;
			return true;
		}
		
		fprintf(stderr, "\nUnrecognised LFO type : '%s'\n", value.c_str());
		return false;
	}

	fprintf(stderr, "Unrecognised LFO setting : '%s'\n", nixers[1].name.c_str());
	return false;
}

bool ProcessReverb(patch* pPatch, vector<NameIndexer> nixers, string value)
{
	if (nixers[1].name == "Enabled")
	{
		pPatch->Reverb.enabled = (value == "1");
		return true;
	}

	if (nixers[1].name == "Feedback")
	{
		pPatch->Reverb.feedback = atof(value.c_str());
		return true;
	}

	if (nixers[1].name == "SampleLength")
	{
		pPatch->Reverb.sampleLength = atoi(value.c_str());
		return true;
	}

	if (nixers[1].name == "Level")
	{
		pPatch->Reverb.level = atof(value.c_str());
		return true;
	}

	fprintf(stderr, "Unrecognised Reverb setting : '%s'\n", nixers[1].name.c_str());
	return false;
}

bool ProcessSetting(patch* pPatch, vector<NameIndexer> nixers, string value, bool& wasSampleSetChange)
{
	/*for (int count=0; count<nixers.size(); count++)
	{
		fprintf(stderr, "nix name = '%s'\n", nixers[count].name.c_str());
		fprintf(stderr, "index = '%i'\n", nixers[count].indexer);
	}

	fprintf(stderr, "value = '%s'\n", value.c_str()); */

	// Process settings (top levels)
	if (nixers[0].name == "LFO")
	{
		return ProcessLFO(pPatch, nixers[0].indexer, nixers, value);
	}

	if (nixers[0].name == "WG")
	{
		return ProcessWG(pPatch, nixers[0].indexer, nixers, value, wasSampleSetChange);
	}

	if (nixers[0].name == "Reverb")
	{
		return ProcessReverb(pPatch, nixers, value);
	}

	if (nixers[0].name == "WGMixMode")
	{
		if (value == "WG0")
		{
			pPatch->WGMixMode = WG0;
			return true;
		}

		if (value == "WG1")
		{
			pPatch->WGMixMode = WG1;
			return true;
		}

		if (value == "WG2")
		{
			pPatch->WGMixMode = WG2;
			return true;
		}

		if (value == "WG3")
		{
			pPatch->WGMixMode = WG3;
			return true;
		}

		if (value == "WG4")
		{
			pPatch->WGMixMode = WG4;
			return true;
		}

		if (value == "WG5")
		{
			pPatch->WGMixMode = WG5;
			return true;
		}

		if (value == "WG0")
		{
			pPatch->WGMixMode = WG0;
			return true;
		}

		if (value == "WG6")
		{
			pPatch->WGMixMode = WG6;
			return true;
		}

		if (value == "WG7")
		{
			pPatch->WGMixMode = WG7;
			return true;
		}

		if (value == "LTOR")
		{
			pPatch->WGMixMode = LTOR;
			return true;
		}

		if (value == "MIX")
		{
			pPatch->WGMixMode = MIX;
			return true;
		}

		if (value == "RING")
		{
			pPatch->WGMixMode = RING;
			return true;
		}

		if (value == "RING_MIX")
		{
			pPatch->WGMixMode = RING_MIX;
			return true;
		}

		fprintf(stderr, "\nUnrecognised WGMixMode : '%s'\n\n", value.c_str());
		return false;
	}

	fprintf(stderr, "Unrecognised top level name : '%s'\n", nixers[0].name.c_str());

	return false; // Could not process
}

bool ProcessPatchLine(patch* pPatch, const string& s, bool& wasSampleSetChange)
{
	//fprintf(stderr, "Process patch line %s\n", s.c_str());

	// Remove any comments
	string findchar = "#";

	vector<string> splits = split(s, findchar);

	// Get the = location
	findchar = "=";
	splits = split(s, findchar);

	if (splits.size() != 2)
	{
		// Must only have 1 =, name and value
		return true;
	}
	
	string name = removechars(splits[0], "	 ");
	string value = removechars(splits[1], " \n\r\t");

	// split the name by . to get the levels
	findchar = ".";
	vector<string> nameParts = split(name, findchar);

	// split the names into names and indexes
	vector<NameIndexer> nameIndexList;

	for (int count=0; count<nameParts.size(); count++)
	{
		findchar = "[";
		splits = split(nameParts[count], findchar);

		NameIndexer nixer;
		nixer.indexer = -1;

		if (splits.size() == 0)
		{
			// blank line
			return true;
		}

		if (splits.size() > 2)
		{
			fprintf(stderr, "Ignored line. Too many '['s: %s\n", nameParts[count].c_str());
			return false;
		}

		nixer.name = removechars(splits[0], "	");
		
		if (splits.size() == 2)
		{
			string index = removechars(splits[1], "]	");
			nixer.indexer = atoi(index.c_str());
		}

		nameIndexList.push_back(nixer);
	}

	return ProcessSetting(pPatch, nameIndexList, value, wasSampleSetChange);
}

bool ReadPatchFromFile(patch* pPatch, char* filePath)
{
	ifstream ifs(filePath);

	if (ifs)
	{
		string line;

		bool sucess = true;

		while( getline(ifs, line) )
		{
			bool wasSampleSetChange;
			bool res = ProcessPatchLine(pPatch, line, wasSampleSetChange);

			if (!res)
			{
				fprintf(stderr, "Patch line %s failed\n", line.c_str());
			}

			sucess &= res;
		}

		if (sucess)
		{
			fprintf(stderr, "*");
		}
		else
		{
			fprintf(stderr, "Patch '%s' Done - with errors\n", filePath);
		}

		ifs.close();

		return sucess;
	}
	else
	{
		fprintf(stderr, "Patch '%s'  does not exist - using default\n", filePath);

		PopulateDefaultPatch(pPatch);
		return false;
	}
}


// Saves a patch to the named file
void SavePatch(synth* pSynth, patch* pPatch, char* filePath)
{
	fprintf(stderr, "Writing patch '%s' - ", filePath);

	FILE * file = fopen (filePath,"w");

	if (file)
	{
		fprintf (file, "# *************** Patch File ***************************\r\n");
		
		fprintf (file, "\r\n\r\n");

		fprintf (file, "# *************** LFOs  ***************************\r\n");

		for (int count=0; count<NUMBER_OF_LFO_FOR_SYNTH; count++)
		{
			fprintf (file, "\r\n");

			fprintf (file, "LFO[%i].Frequency=%f\r\n", count, pPatch->LFOs[count].frequency);		
		
			switch (pPatch->LFOs[count]._type)
			{
				case MUTE:
					fprintf (file, "LFO[%i].Type=MUTE\r\n", count);	
					break;

				case SQUARE:
					fprintf (file, "LFO[%i].Type=SQUARE\r\n", count);	
					break;

				case SIN:
					fprintf (file, "LFO[%i].Type=SIN\r\n", count);	
					break;

				case SAW:
					fprintf (file, "LFO[%i].Type=SAW\r\n", count);	
					break;

				case NOISE:
					fprintf (file, "LFO[%i].Type=NOISE\r\n", count);	
					break;

				case RND_SQ:
					fprintf (file, "LFO[%i].Type=RND_SQ\r\n", count);	
					break;

				case PCM:
					fprintf (file, "LFOWG[%i].Type=PCM\r\n", count);	
					break;

				default:
					fprintf (file, "LFO[%i].Type=******** UNKNOWN *******\r\n", count);
					fprintf(stderr, "Unknown LFO type '%i'\r\n", pPatch->LFOs[count]._type);
					break;
			}
		}


		fprintf (file, "\r\n\r\n");

		fprintf (file, "# *************** Wave generators  ***************************\r\n");
		for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
		{
			fprintf (file, "\r\n");

			patchWG wg = pPatch->WGs[count];

			switch (wg._type)
			{
				case MUTE:
					fprintf (file, "WG[%i].Type=MUTE\r\n", count);	
					break;

				case SQUARE:
					fprintf (file, "WG[%i].Type=SQUARE\r\n", count);	
					break;

				case SIN:
					fprintf (file, "WG[%i].Type=SIN\r\n", count);	
					break;

				case SAW:
					fprintf (file, "WG[%i].Type=SAW\r\n", count);	
					break;

				case NOISE:
					fprintf (file, "WG[%i].Type=NOISE\r\n", count);	
					break;

				case RND_SQ:
					fprintf (file, "WG[%i].Type=RND_SQ\r\n", count);	
					break;

				case PCM:
					fprintf (file, "WG[%i].Type=PCM\r\n", count);	
					break;

				default:
					fprintf (file, "WG[%i].Type=***** UNKNOWN *******\r\n", count);
					fprintf(stderr, "Unknown WG type '%i' - ", pPatch->LFOs[count]._type);
					break;
			}

			if (wg.enablePitchBend)
			{
				fprintf (file, "WG[%i].EnablePitchBend=true\r\n", count);
			}
			else
			{
				fprintf (file, "WG[%i].EnablePitchBend=false\r\n", count);
			}

			fprintf (file, "WG[%i].PitchBendAmount=%f\r\n", count, wg.pitchBendAmount);
			fprintf (file, "WG[%i].VelocityVolumeAdjust=%f\r\n", count, wg.velocityVolumeAdjust);

			
			fprintf (file, "WG[%i].KeyOffestSemitones=%f\r\n", count, wg.keyOffestSemitones);
			fprintf (file, "WG[%i].FreqLFOid=%i\r\n", count, wg.freqLFOid);
			fprintf (file, "WG[%i].FreqLFOLevel=%f\r\n", count, wg.freqLFOLevel);
			fprintf (file, "WG[%i].FreqLFODelay=%f\r\n", count, wg.freqLFODelay);
			fprintf (file, "WG[%i].PWMLFOid=%i\r\n", count, wg.pwmLFOid);
			fprintf (file, "WG[%i].PWMLFOLevel=%f\r\n", count, wg.pwmLFOid);

			fprintf (file, "\r\n\r\n");

			fprintf (file, "# ****************** Filter *******************\r\n");
			switch (wg.Filter.FilterType)
			{
				case OFF:
					fprintf (file, "WG[%i].Filter.FilterType=OFF\r\n", count);
					break;

				case LPF:
					fprintf (file, "WG[%i].Filter.FilterType=LPF\r\n", count);
					break;

				case HPF:
					fprintf (file, "WG[%i].Filter.FilterType=HPF\r\n", count);
					break;

				case BPF1:
					fprintf (file, "WG[%i].Filter.FilterType=BPF1\r\n", count);
					break;

				case BPF2:
					fprintf (file, "WG[%i].Filter.FilterType=BPF2\r\n", count);
					break;

				case NOTCH:
					fprintf (file, "WG[%i].Filter.FilterType=NOTCH\r\n", count);
					break;

				case APF:
					fprintf (file, "WG[%i].Filter.FilterType=APF\r\n", count);
					break;

				case PEAKINGEQ:
					fprintf (file, "WG[%i].Filter.FilterType=PEAKINGEQ\r\n", count);
					break;

				case LOWSHELF:
					fprintf (file, "WG[%i].Filter.FilterType=LOWSHELF\r\n", count);
					break;

				case HIGHSHELF:
					fprintf (file, "WG[%i].Filter.FilterType=HIGHSHELF\r\n", count);
					break;
			}

			fprintf (file, "WG[%i].Filter.Q=%f\r\n", count, wg.Filter.Q);
			fprintf (file, "WG[%i].Filter.RelativeFrequency=%f\r\n", count, wg.Filter.RelativeFrequency);
			fprintf (file, "WG[%i].Filter.DBGain=%f\r\n", count, wg.Filter.DBGain);
			fprintf (file, "WG[%i].Filter.VelocityFactor=%f\r\n", count, wg.Filter.VelocityFactor);
			

			fprintf (file, "# ****************** Sampleset *******************\r\n");


			fprintf (file, "WG[%i].SampleSet.NSamples=%i\r\n", wg.SampleSet.NSamples);

			for (int count2=0 ; count2<wg.SampleSet.NSamples; count2++)
			{
				fprintf (file, "WG[%i].SampleSet.Sample[%i].File=%s\r\n", count, count2, wg.SampleSet.FileNames[count2].c_str());
				fprintf (file, "WG[%i].SampleSet.Sample[%i].SampledNoteFrequency=%f\r\n", count, count2, wg.SampleSet.SampledNoteFrequency[count2]);
				fprintf (file, "WG[%i].SampleSet.Sample[%i].LoopStartSample=%i\r\n", count, count2, wg.SampleSet.LoopStartSample[count2]);
				fprintf (file, "WG[%i].SampleSet.Sample[%i].LoopEndSample=%i\r\n", count, count2, wg.SampleSet.LoopEndSample[count2]);
			}
		}

		fprintf (file, "\r\n\r\n");

#ifdef ENABLE_TDP
		fprintf (file, "# ****** Time dependant pitch adjustment *******************\r\n");
		fprintf (file, "# Levels in semitones\r\n");
		fprintf (file, "#****************************************************************\r\n");

		for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
		{
			fprintf (file, "\r\n");

			patchWG wg = pPatch->WGs[count];

			fprintf (file, "WG[%i].TDP.L0=%f\r\n", count, wg.TDP.L0);
			fprintf (file, "WG[%i].TDP.T1=%f\r\n", count, wg.TDP.T1);
			fprintf (file, "WG[%i].TDP.L1=%f\r\n", count, wg.TDP.L1);
			fprintf (file, "WG[%i].TDP.T2=%f\r\n", count, wg.TDP.T2);
			fprintf (file, "WG[%i].TDP.L2=%f\r\n", count, wg.TDP.L2);
			fprintf (file, "WG[%i].TDP.T3=%f\r\n", count, wg.TDP.T3);
			fprintf (file, "WG[%i].TDP.Sustain=%f\r\n", count, wg.TDP.Sustain);
			fprintf (file, "WG[%i].TDP.T4=%f\r\n", count, wg.TDP.T4);
			fprintf (file, "WG[%i].TDP.L4=%f\r\n", count, wg.TDP.L4);
		}

		fprintf (file, "\r\n\r\n");
#endif

#ifdef ENABLE_TDA
		fprintf (file, "# ****************** Time dependant amplifier *******************\r\n");
		fprintf (file, "# Levels in gain 0.0F to 1.0F\r\n");
		fprintf (file, "#****************************************************************\r\n");

		for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
		{
			fprintf (file, "\r\n");

			patchWG wg = pPatch->WGs[count];

			fprintf (file, "WG[%i].TDA.L0=%f\r\n", count, wg.TDA.L0);
			fprintf (file, "WG[%i].TDA.T1=%f\r\n", count, wg.TDA.T1);
			fprintf (file, "WG[%i].TDA.L1=%f\r\n", count, wg.TDA.L1);
			fprintf (file, "WG[%i].TDA.T2=%f\r\n", count, wg.TDA.T2);
			fprintf (file, "WG[%i].TDA.L2=%f\r\n", count, wg.TDA.L2);
			fprintf (file, "WG[%i].TDA.T3=%f\r\n", count, wg.TDA.T3);
			fprintf (file, "WG[%i].TDA.Sustain=%f\r\n", count, wg.TDA.Sustain);
			fprintf (file, "WG[%i].TDA.T4=%f\r\n", count, wg.TDA.T4);
			fprintf (file, "WG[%i].TDA.L4=%f\r\n", count, wg.TDA.L4);
		}

		fprintf (file, "\r\n\r\n");
#endif

#ifdef ENABLE_TDF
		fprintf (file, "# ****** Time dependant filter adjustment *******************\r\n");
		fprintf (file, "# Levels in semitines\r\n");
		fprintf (file, "#****************************************************************\r\n");

		for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
		{
			fprintf (file, "\r\n");

			patchWG wg = pPatch->WGs[count];

			fprintf (file, "WG[%i].TDF.L0=%f\r\n", count, wg.TDP.L0);
			fprintf (file, "WG[%i].TDF.T1=%f\r\n", count, wg.TDP.T1);
			fprintf (file, "WG[%i].TDF.L1=%f\r\n", count, wg.TDP.L1);
			fprintf (file, "WG[%i].TDF.T2=%f\r\n", count, wg.TDP.T2);
			fprintf (file, "WG[%i].TDF.L2=%f\r\n", count, wg.TDP.L2);
			fprintf (file, "WG[%i].TDF.T3=%f\r\n", count, wg.TDP.T3);
			fprintf (file, "WG[%i].TDF.Sustain=%f\r\n", count, wg.TDP.Sustain);
			fprintf (file, "WG[%i].TDF.T4=%f\r\n", count, wg.TDP.T4);
			fprintf (file, "WG[%i].TDF.L4=%f\r\n", count, wg.TDP.L4);
		}

		fprintf (file, "\r\n\r\n");
#endif		

		fprintf (file, "#****************** Voice Settings ****************************\r\n");
		fprintf (file, "# \r\n");
		fprintf (file, "#****************************************************************\r\n");
		
		switch (pPatch->WGMixMode)
		{
			case	WG0:
			case	WG1:
			case	WG2:
			case	WG3:
			case	WG4:
			case	WG5:
			case	WG6:
			case	WG7:
				fprintf (file, "WGMixMode=WG%i\r\n", (int) pPatch->WGMixMode);
				break;
			
			case LTOR:
				fprintf (file, "WGMixMode=LTOR\r\n");
				break;

			case MIX:
				fprintf (file, "WGMixMode=MIX\r\n");
				break;

			case RING:
				fprintf (file, "WGMixMode=RING\r\n");
				break;

			case RING_MIX:
				fprintf (file, "WGMixMode=RING_MIX\r\n");
				break;
		}

		fprintf (file, "\r\n\r\n");

		fprintf (file, "# ****** Reverb settings *******************\r\n");
		fprintf (file, "# Feedback and Level should be less that 1.0 \r\n");
		fprintf (file, "#****************************************************************\r\n");

		if (pPatch->Reverb.enabled)
		{
			fprintf (file, "Reverb.Enabled=1\r\n");
		}
		else
		{
			fprintf (file, "Reverb.Enabled=0\r\n");
		}

		fprintf (file, "Reverb.Feedback=%f\r\n", pPatch->Reverb.feedback);
		fprintf (file, "Reverb.SampleLength=%i\r\n", pPatch->Reverb.sampleLength);
		fprintf (file, "Reverb.Level=%f\r\n", pPatch->Reverb.level);

		fprintf (file, "\r\n\r\n");
		fprintf (file, "# ************** END OF FILE *******************\n");

		fclose(file);

		fprintf(stderr, "Done\n");
	}
	else
	{
		fprintf(stderr, "Error writing file\n");
	}
}

// Populates a default patch, useful for testing and for missing patch files
void PopulateDefaultPatch(patch* pPatch)
{
	// LFOS
	for (int count=0; count<NUMBER_OF_LFO_FOR_SYNTH; count++)
	{
		pPatch->LFOs[count].frequency = 0.25F;
		pPatch->LFOs[count]._type = SIN;
	}

	for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
	{
		// Wave generators
		switch (count)
		{
			case 0:
				pPatch->WGs[count]._type = SQUARE;
				pPatch->WGs[count].keyOffestSemitones = 0.0F;
				break;
				
			case 1:
				pPatch->WGs[count]._type = SIN;
				pPatch->WGs[count].keyOffestSemitones = 4.0F;
				break;
				
		}
		pPatch->WGs[count].enablePitchBend = true;
		pPatch->WGs[count].pitchBendAmount = 1.0F;

		
		pPatch->WGs[count].freqLFOid = 0;
		pPatch->WGs[count].freqLFOLevel = 0.0F;
		pPatch->WGs[count].freqLFODelay = 0.0F;
		pPatch->WGs[count].pwmLFOid = 0;
		pPatch->WGs[count].pwmLFOLevel = 0.9F;

		// **************** Filter *******************
		pPatch->WGs[count].Filter.FilterType = LPF;
		pPatch->WGs[count].Filter.Q = 1;
		pPatch->WGs[count].Filter.RelativeFrequency = 1.0F;
		pPatch->WGs[count].Filter.DBGain = 0.0F;
	

#ifdef ENABLE_TDP
		//********************** TDPs *********************
		// Levels in semitones 
		pPatch->WGs[count].TDP.L0 = 0.0F;
		pPatch->WGs[count].TDP.T1 = 0.0F;
		pPatch->WGs[count].TDP.L1 = 0.0F;
		pPatch->WGs[count].TDP.T2 = 0.2F;
		pPatch->WGs[count].TDP.L2 = 0.0F;
		pPatch->WGs[count].TDP.T3 = 0.0F;
		pPatch->WGs[count].TDP.Sustain = 0.0F;
		pPatch->WGs[count].TDP.T4 = 1.0F;
		pPatch->WGs[count].TDP.L4 = 0.0F;
#endif

#ifdef ENABLE_TDA
		// ********************* TDAs *********************
		// Levels in gain 0.0F to 1.0F
		pPatch->WGs[count].TDA.L0 = 1.0F;
		pPatch->WGs[count].TDA.T1 = 1.0F;
		pPatch->WGs[count].TDA.L1 = 0.5F;
		pPatch->WGs[count].TDA.T2 = 1.0F;
		pPatch->WGs[count].TDA.L2 = 0.4F;
		pPatch->WGs[count].TDA.T3 = 1.0F;
		pPatch->WGs[count].TDA.Sustain = 0.1F;
		pPatch->WGs[count].TDA.T4 = 2.0F;
		pPatch->WGs[count].TDA.L4 = 0.0F;
#endif

#ifdef ENABLE_TDF
		// ********************* TDFs *********************
		// Levels in semitones
		pPatch->WGs[count].TDF.L0 = 0.0F;
		pPatch->WGs[count].TDF.T1 = 0.0F;
		pPatch->WGs[count].TDF.L1 = 0.0F;
		pPatch->WGs[count].TDF.T2 = 0.0F;
		pPatch->WGs[count].TDF.L2 = 0.0F;
		pPatch->WGs[count].TDF.T3 = 0.0F;
		pPatch->WGs[count].TDF.Sustain = 0.0F;
		pPatch->WGs[count].TDF.T4 = 0.0F;
		pPatch->WGs[count].TDF.L4 = 0.0F;
#endif
	}

	pPatch->WGMixMode = MIX;

	// Reverb
	pPatch->Reverb.feedback = 0.60F;
	pPatch->Reverb.sampleLength = 4000;
	pPatch->Reverb.level = 0.08F;

}
