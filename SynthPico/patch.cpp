#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <math.h>

#include "signal.h"
#include "constants.h"
#include "patch.h"


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

	if (nixers[2].name == "L1")
	{
		pTDL->L1 = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "L2")
	{
		pTDL->L2 = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "Sustain")
	{
		pTDL->Sustain = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "L4")
	{
		pTDL->L4 = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "T1")
	{
		pTDL->T1 = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "T2")
	{
		pTDL->T1 = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "T3")
	{
		pTDL->T1 = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "T4")
	{
		pTDL->T1 = signal( (float) atof(value.c_str()));
		return true;
	}

	fprintf(stderr, "Unrecognised TD(A/F) setting : '%s'\n", nixers[2].name.c_str());
	return false;
}

bool ProcessFilter(patchFilter* pFilter, vector<NameIndexer> nixers, string value)
{
	if (nixers[2].name == "FilterType")
	{
		if (value == "OFF")
		{
			pFilter->FilterType= filtertype::OFF;
			return true;
		}

		if (value == "LPF")
		{
			pFilter->FilterType= filtertype::LPF;
			return true;
		}

		if (value == "HPF")
		{
			pFilter->FilterType= filtertype::HPF;
			return true;
		}

		if (value == "BPF")
		{
			pFilter->FilterType= filtertype::BPF;
			return true;
		}

		if (value == "BPF2")
		{
			pFilter->FilterType= filtertype::BPF2;
			return true;
		}

		if (value == "NOTCH")
		{
			pFilter->FilterType= filtertype::NOTCH;
			return true;
		}

		if (value == "APF")
		{
			pFilter->FilterType= filtertype::APF;
			return true;
		}

		if (value == "PEAKINGEQ")
		{
			pFilter->FilterType= filtertype::PEAKINGEQ;
			return true;
		}

		if (value == "LOWSHELF")
		{
			pFilter->FilterType= filtertype::LOWSHELF;
			return true;
		}

		fprintf(stderr, "\nUnrecognised FilterType : '%s'\n\n", value.c_str());
		return false;
	}

	if (nixers[2].name == "Q")
	{
		pFilter->Q = signal((float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "RelativeFrequency")
	{
		pFilter->RelativeFrequency = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "DBGain")
	{
		pFilter->DBGain = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[2].name == "VelocityFactor")
	{
		pFilter->VelocityFactor = signal( (float) atof(value.c_str()));
		return true;
	}

	fprintf(stderr, "Unrecognised filter setting : '%s'\n", nixers[2].name.c_str());
	return false;
}

bool wgLimitWarning = false; // so we only print warning once

bool ProcessWG(int WGIndex, vector<NameIndexer> nixers, string value)
{
	if (WGIndex >= NUMBER_OF_WAVE_GENERATORS_PER_VOICE && !wgLimitWarning)
	{
		fprintf(stderr, "Only %i wave generators supported in this build, requested WG[%i]\n", NUMBER_OF_WAVE_GENERATORS_PER_VOICE, WGIndex);
		wgLimitWarning = true;
		return false;
	}

	patchWG* pWG = &GPatch.WGs[WGIndex];

	if (nixers[1].name == "Type")
	{
		if (value == "MUTE")
		{
			pWG->_type = waveformtype::MUTE;
			return true;
		}

		if (value == "SQUARE")
		{
			pWG->_type = waveformtype::SQUARE;
			return true;
		}

		if (value == "SIN")
		{
			pWG->_type = waveformtype::SIN;
			return true;
		}

		if (value == "SAW")
		{
			pWG->_type = waveformtype::SAW;
			return true;
		}

		if (value == "NOISE")
		{
			pWG->_type = waveformtype::NOISE;
			return true;
		}

		if (value == "RND_SQ")
		{
			pWG->_type = waveformtype::RND_SQ;
			return true;
		}

		fprintf(stderr, "\nUnrecognised WG type : '%s'\n\n", value.c_str());
		return false;
	}

	if (nixers[1].name == "PWMLFOid")
	{
		pWG->pwmLFOid = atoi(value.c_str());
		return true;
	}

	if (nixers[1].name == "PWMLFOLevel")
	{
		pWG->pwmLFOLevel = signal ( (float) atof(value.c_str()));
		return true;
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
		pWG->pitchBendAmount = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[1].name == "FreqLFOid")
	{
		pWG->freqLFOid = atoi(value.c_str());
		return true;
	}

	if (nixers[1].name == "FreqLFOLevel")
	{
		pWG->freqLFOLevel = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[1].name == "FreqLFODelay")
	{
		pWG->freqLFODelay = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[1].name == "FixedPanAdjustment")
	{
		pWG->fixedPanAdjustment = signal((float)atof(value.c_str()));
		return true;
	}

	if (nixers[1].name == "KeyPanAdjustment")
	{
		pWG->keyPanAdjustment = signal( (float) atof(value.c_str()));
		return true;
	}
	
	if (nixers[1].name == "KeyOffestSemitones")
	{
		pWG->keyOffestSemitones =  (float) atof(value.c_str());
		return true;
	}





#ifdef ENABLE_TDP
	if (nixers[1].name == "TDP")
	{
		return ProcessTDL(&pWG->TDP, nixers, value);
	}
#endif

	if (nixers[1].name == "VelocityVolumeAdjust")
	{
		pWG->velocityVolumeAdjust = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[1].name == "VelocityPanAdjust")
	{
		pWG->velocityPanAdjust = signal( (float) atof(value.c_str()));
		return true;
	}

#ifdef ENABLE_TDA
	if (nixers[1].name == "TDA")
	{
		return ProcessTDL(&pWG->TDA, nixers, value);
	}
#endif

#ifdef ENABLE_TDF
	if (nixers[1].name == "TDF")
	{
		return ProcessTDL(&GPatch.TDF, nixers, value);
	}
#endif

	if (nixers[1].name == "Filter")
	{
		return ProcessFilter(&GPatch.Filter, nixers, value);
	}

	fprintf(stderr, "Unrecognised WG setting : '%s'\n", nixers[1].name.c_str());
	return false;
}

bool ProcessLFO(int LFOIndex, vector<NameIndexer> nixers, string value)
{
	if (nixers[1].name == "Frequency")
	{
		GPatch.LFOs[LFOIndex].frequency = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[1].name == "Type")
	{
		if (value == "MUTE")
		{
			GPatch.LFOs[LFOIndex]._type = waveformtype::MUTE;
			return true;
		}

		if (value == "SQUARE")
		{
			GPatch.LFOs[LFOIndex]._type = waveformtype::SQUARE;
			return true;
		}

		if (value == "SIN")
		{
			GPatch.LFOs[LFOIndex]._type = waveformtype::SIN;
			return true;
		}

		if (value == "SAW")
		{
			GPatch.LFOs[LFOIndex]._type = waveformtype::SAW;
			return true;
		}

		if (value == "NOISE")
		{
			GPatch.LFOs[LFOIndex]._type = waveformtype::NOISE;
			return true;
		}

		if (value == "RND_SQ")
		{
			GPatch.LFOs[LFOIndex]._type = waveformtype::RND_SQ;
			return true;
		}
		
		fprintf(stderr, "\nUnrecognised LFO type : '%s'\n", value.c_str());
		return false;
	}

	fprintf(stderr, "Unrecognised LFO setting : '%s'\n", nixers[1].name.c_str());
	return false;
}

bool ProcessReverb(vector<NameIndexer> nixers, string value)
{
	if (nixers[1].name == "Enabled")
	{
		GPatch.Reverb.enabled = (value == "1");
		return true;
	}

	if (nixers[1].name == "Feedback")
	{
		GPatch.Reverb.feedback = signal( (float) atof(value.c_str()));
		return true;
	}

	if (nixers[1].name == "SampleLength")
	{
		GPatch.Reverb.sampleLength = atoi(value.c_str());
		return true;
	}

	if (nixers[1].name == "Level")
	{
		GPatch.Reverb.level = signal( (float) atof(value.c_str()));
		return true;
	}

	fprintf(stderr, "Unrecognised Reverb setting : '%s'\n", nixers[1].name.c_str());
	return false;
}

bool ProcessSetting(vector<NameIndexer> nixers, string value)
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
		return ProcessLFO(nixers[0].indexer, nixers, value);
	}

	if (nixers[0].name == "WG")
	{
		return ProcessWG(nixers[0].indexer, nixers, value);
	}

	if (nixers[0].name == "Reverb")
	{
		return ProcessReverb(nixers, value);
	}

	if (nixers[0].name == "WGMixMode")
	{
		if (value == "WG0")
		{
			GPatch.WGMixMode = WGMIX::WG0;
			return true;
		}

		if (value == "WG1")
		{
			GPatch.WGMixMode = WGMIX::WG1;
			return true;
		}

		if (value == "WG2")
		{
			GPatch.WGMixMode = WGMIX::WG2;
			return true;
		}

		if (value == "WG3")
		{
			GPatch.WGMixMode = WGMIX::WG3;
			return true;
		}

		if (value == "WG4")
		{
			GPatch.WGMixMode = WGMIX::WG4;
			return true;
		}

		if (value == "WG5")
		{
			GPatch.WGMixMode = WGMIX::WG5;
			return true;
		}

		if (value == "WG0")
		{
			GPatch.WGMixMode = WGMIX::WG0;
			return true;
		}

		if (value == "WG6")
		{
			GPatch.WGMixMode = WGMIX::WG6;
			return true;
		}

		if (value == "WG7")
		{
			GPatch.WGMixMode = WGMIX::WG7;
			return true;
		}

		if (value == "MIX")
		{
			GPatch.WGMixMode = WGMIX::MIX;
			return true;
		}

		if (value == "RING")
		{
			GPatch.WGMixMode = WGMIX::RING;
			return true;
		}

		if (value == "RING_MIX")
		{
			GPatch.WGMixMode = WGMIX::RING_MIX;
			return true;
		}

		fprintf(stderr, "\nUnrecognised WGMixMode : '%s'\n\n", value.c_str());
		return false;
	}

	fprintf(stderr, "Unrecognised top level name : '%s'\n", nixers[0].name.c_str());

	return false; // Could not process
}

bool ProcessPatchLine(const string& s)
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

	return ProcessSetting(nameIndexList, value);
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
				case waveformtype::MUTE:
					fprintf (file, "LFO[%i].Type=MUTE\r\n", count);	
					break;

				case waveformtype::SQUARE:
					fprintf (file, "LFO[%i].Type=SQUARE\r\n", count);	
					break;

				case waveformtype::SIN:
					fprintf (file, "LFO[%i].Type=SIN\r\n", count);	
					break;

				case waveformtype::SAW:
					fprintf (file, "LFO[%i].Type=SAW\r\n", count);	
					break;

				case waveformtype::NOISE:
					fprintf (file, "LFO[%i].Type=NOISE\r\n", count);	
					break;

				case waveformtype::RND_SQ:
					fprintf (file, "LFO[%i].Type=RND_SQ\r\n", count);	
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
				case waveformtype::MUTE:
					fprintf (file, "WG[%i].Type=MUTE\r\n", count);	
					break;

				case waveformtype::SQUARE:
					fprintf (file, "WG[%i].Type=SQUARE\r\n", count);	
					break;

				case waveformtype::SIN:
					fprintf (file, "WG[%i].Type=SIN\r\n", count);	
					break;

				case waveformtype::SAW:
					fprintf (file, "WG[%i].Type=SAW\r\n", count);	
					break;

				case waveformtype::NOISE:
					fprintf (file, "WG[%i].Type=NOISE\r\n", count);	
					break;

				case waveformtype::RND_SQ:
					fprintf (file, "WG[%i].Type=RND_SQ\r\n", count);	
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

			// Pan settings
			fprintf(file, "WG[%i].VelocityPanAdjust=%f\r\n", count, wg.velocityPanAdjust);
			fprintf(file, "WG[%i].FixedPanAdjustment=%f\r\n", count, wg.fixedPanAdjustment);
			fprintf(file, "WG[%i].KeyPanAdjustment=%f\r\n", count, wg.keyPanAdjustment);
			
			fprintf (file, "WG[%i].KeyOffestSemitones=%f\r\n", count, wg.keyOffestSemitones);
			fprintf (file, "WG[%i].FreqLFOid=%i\r\n", count, wg.freqLFOid);
			fprintf (file, "WG[%i].FreqLFOLevel=%f\r\n", count, wg.freqLFOLevel);
			fprintf (file, "WG[%i].FreqLFODelay=%f\r\n", count, wg.freqLFODelay);
			fprintf (file, "WG[%i].PWMLFOid=%i\r\n", count, wg.pwmLFOid);
			fprintf (file, "WG[%i].PWMLFOLevel=%i\r\n", count, wg.pwmLFOid);

			fprintf (file, "\r\n\r\n");

			fprintf (file, "# ****************** Filter *******************\r\n");
			switch (GPatch.Filter.FilterType)
			{
			case filtertype::OFF:
					fprintf (file, "Filter.FilterType=OFF\r\n", count);
					break;

			case filtertype::LPF:
					fprintf (file, "Filter.FilterType=LPF\r\n", count);
					break;

			case filtertype::HPF:
					fprintf (file, "Filter.FilterType=HPF\r\n", count);
					break;

			case filtertype::BPF:
					fprintf (file, "Filter.FilterType=BPF\r\n", count);
					break;

			case filtertype::BPF2:
					fprintf (file, "Filter.FilterType=BPF2\r\n", count);
					break;

			case filtertype::NOTCH:
					fprintf (file, "Filter.FilterType=NOTCH\r\n", count);
					break;

			case filtertype::APF:
					fprintf (file, "Filter.FilterType=APF\r\n", count);
					break;

			case filtertype::PEAKINGEQ:
					fprintf (file, "Filter.FilterType=PEAKINGEQ\r\n", count);
					break;

			case filtertype::LOWSHELF:
					fprintf (file, "Filter.FilterType=LOWSHELF\r\n", count);
					break;

			case filtertype::HIGHSHELF:
					fprintf (file, "Filter.FilterType=HIGHSHELF\r\n", count);
					break;
			}

			fprintf (file, "Filter.Q=%f\r\n", count, GPatch.Filter.Q);
			fprintf (file, "Filter.RelativeFrequency=%f\r\n", count, GPatch.Filter.RelativeFrequency);
			fprintf (file, "Filter.DBGain=%f\r\n", count, GPatch.Filter.DBGain);
			fprintf (file, "Filter.VelocityFactor=%f\r\n", count, GPatch.Filter.VelocityFactor);
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
			case WGMIX::WG0:
			case WGMIX::WG1:
			case WGMIX::WG2:
			case WGMIX::WG3:
			case WGMIX::WG4:
			case WGMIX::WG5:
			case WGMIX::WG6:
			case WGMIX::WG7:
				fprintf (file, "WGMixMode=WG%i\r\n", (int) pPatch->WGMixMode);
				break;
			
			case WGMIX::MIX:
				fprintf (file, "WGMixMode=MIX\r\n");
				break;

			case WGMIX::RING:
				fprintf (file, "WGMixMode=RING\r\n");
				break;

			case WGMIX::RING_MIX:
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
void PopulateDefaultPatch()
{
	printf("PopulateDefaultPatch\n");

	// LFOS
	for (int count=0; count<NUMBER_OF_LFO_FOR_SYNTH; count++)
	{
		switch (count)
		{
			case 0:
				GPatch.LFOs[count].frequency = 3.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 1:
				GPatch.LFOs[count].frequency = 5.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 2:
				GPatch.LFOs[count].frequency = 4.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 3:
				GPatch.LFOs[count].frequency = 2.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 4:
				GPatch.LFOs[count].frequency = 1.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 5:
				GPatch.LFOs[count].frequency = 1.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 6:
				GPatch.LFOs[count].frequency = 1.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 7:
				GPatch.LFOs[count].frequency = 1.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

			case 8:
				GPatch.LFOs[count].frequency = 1.0F;
				GPatch.LFOs[count]._type = waveformtype::RND_SQ;
				break;

		}
	}

	float freqdev = 0.1F;

	for (int count=0; count<NUMBER_OF_WAVE_GENERATORS_PER_VOICE; count++)
	{
		//GPatch.WGs[count].mixLevel = 1.0F;
		//GPatch.WGs[count].fmLevel = 0.0F;

		// Wave generators
		switch (count)
		{
			case 0:
				GPatch.WGs[count]._type = waveformtype::SAW;
				GPatch.WGs[count].enablePitchBend = true;
				GPatch.WGs[count].pitchBendAmount = 1.0F;
				GPatch.WGs[count].velocityVolumeAdjust = 0.5F;

				GPatch.WGs[count].velocityPanAdjust = 0.0F;
				GPatch.WGs[count].fixedPanAdjustment = -0.3F;
				GPatch.WGs[count].keyPanAdjustment = 1.0F;

				GPatch.WGs[count].keyOffestSemitones = 12.0F;

				GPatch.WGs[count].freqLFOid = count;
				GPatch.WGs[count].freqLFOLevel = freqdev;
				GPatch.WGs[count].freqLFODelay = 0.0F;
				break;
				
			case 1:
				GPatch.WGs[count]._type = waveformtype::SAW;
				GPatch.WGs[count].enablePitchBend = true;
				GPatch.WGs[count].pitchBendAmount = 1.0F;
				GPatch.WGs[count].velocityVolumeAdjust = 0.5F;

				GPatch.WGs[count].velocityPanAdjust = 0.0F;
				GPatch.WGs[count].fixedPanAdjustment = -0.2F;
				GPatch.WGs[count].keyPanAdjustment = 1.0F;

				GPatch.WGs[count].keyOffestSemitones = 12.0F;

				GPatch.WGs[count].freqLFOid = count;
				GPatch.WGs[count].freqLFOLevel = freqdev;
				GPatch.WGs[count].freqLFODelay = 0.0F;
				break;

			case 2:
				GPatch.WGs[count]._type = waveformtype::SAW;
				GPatch.WGs[count].enablePitchBend = true;
				GPatch.WGs[count].pitchBendAmount = 1.0F;
				GPatch.WGs[count].velocityVolumeAdjust = 0.5F;

				GPatch.WGs[count].velocityPanAdjust = 0.0F;
				GPatch.WGs[count].fixedPanAdjustment = -0.1F;
				GPatch.WGs[count].keyPanAdjustment = 1.0F;

				GPatch.WGs[count].keyOffestSemitones = -12.1F;

				GPatch.WGs[count].freqLFOid = count;
				GPatch.WGs[count].freqLFOLevel = freqdev;
				GPatch.WGs[count].freqLFODelay = 0.0F;
				break;

			case 3:
				GPatch.WGs[count]._type = waveformtype::SAW;
				GPatch.WGs[count].enablePitchBend = true;
				GPatch.WGs[count].pitchBendAmount = 1.0F;
				GPatch.WGs[count].velocityVolumeAdjust = 0.5F;

				GPatch.WGs[count].velocityPanAdjust = 0.0F;
				GPatch.WGs[count].fixedPanAdjustment = 0.0F;
				GPatch.WGs[count].keyPanAdjustment = 1.0F;

				GPatch.WGs[count].keyOffestSemitones = -12.0F;

				GPatch.WGs[count].freqLFOid = count;
				GPatch.WGs[count].freqLFOLevel = freqdev;
				GPatch.WGs[count].freqLFODelay = 0.0F;
				break;

			case 4:
				GPatch.WGs[count]._type = waveformtype::SAW;
				GPatch.WGs[count].enablePitchBend = true;
				GPatch.WGs[count].pitchBendAmount = 1.0F;
				GPatch.WGs[count].velocityVolumeAdjust = 0.5F;

				GPatch.WGs[count].velocityPanAdjust = 0.0F;
				GPatch.WGs[count].fixedPanAdjustment = 0.1F;
				GPatch.WGs[count].keyPanAdjustment = 1.0F;

				GPatch.WGs[count].keyOffestSemitones = -12.01F;

				GPatch.WGs[count].freqLFOid = count;
				GPatch.WGs[count].freqLFOLevel = freqdev;
				GPatch.WGs[count].freqLFODelay = 0.0F;
				break;

			case 5:
				GPatch.WGs[count]._type = waveformtype::SAW;
				GPatch.WGs[count].enablePitchBend = true;
				GPatch.WGs[count].pitchBendAmount = 1.0F;
				GPatch.WGs[count].velocityVolumeAdjust = 0.5F;

				GPatch.WGs[count].velocityPanAdjust = 0.0F;
				GPatch.WGs[count].fixedPanAdjustment = 0.2F;
				GPatch.WGs[count].keyPanAdjustment = 1.0F;

				GPatch.WGs[count].keyOffestSemitones = -11.08F;

				GPatch.WGs[count].freqLFOid = count;
				GPatch.WGs[count].freqLFOLevel = freqdev;
				GPatch.WGs[count].freqLFODelay = 0.0F;
				break;

				
		}
		GPatch.WGs[count].enablePitchBend = true;
		GPatch.WGs[count].pitchBendAmount = 1.0F;

		GPatch.WGs[count].pwmLFOid = 0;
		GPatch.WGs[count].pwmLFOLevel = 0.8F;


#ifdef ENABLE_TDP
		switch (count)
		{
			case 0:
				//********************** TDPs *********************
				// Pitch: Levels in semitones 
				GPatch.WGs[count].TDP.T1 = 0.0F;
				GPatch.WGs[count].TDP.L1 = 0.0F;
				GPatch.WGs[count].TDP.T2 = 10.0F;
				GPatch.WGs[count].TDP.L2 = 12.0F;
				GPatch.WGs[count].TDP.T3 = 10.0F;
				GPatch.WGs[count].TDP.Sustain = -12.0F;
				GPatch.WGs[count].TDP.T4 = 0.0F;
				GPatch.WGs[count].TDP.L4 = 0.0F;
				break;
				
			case 1:
				//********************** TDPs *********************
				// Pitch: Levels in semitones 
				GPatch.WGs[count].TDP.T1 = 0.0F;
				GPatch.WGs[count].TDP.L1 = 0.0F;
				GPatch.WGs[count].TDP.T2 = 0.0F;
				GPatch.WGs[count].TDP.L2 = 0.0F;
				GPatch.WGs[count].TDP.T3 = 0.0F;
				GPatch.WGs[count].TDP.Sustain = 0.0F;
				GPatch.WGs[count].TDP.T4 = 0.0F;
				GPatch.WGs[count].TDP.L4 = 0.0F;
				break;

			case 2:
				//********************** TDPs *********************
				// Pitch: Levels in semitones amidi -l
				GPatch.WGs[count].TDP.T1 = 0.0F;
				GPatch.WGs[count].TDP.L1 = 0.0F;
				GPatch.WGs[count].TDP.T2 = 0.0F;
				GPatch.WGs[count].TDP.L2 = 0.0F;
				GPatch.WGs[count].TDP.T3 = 0.0F;
				GPatch.WGs[count].TDP.Sustain = 0.0F;
				GPatch.WGs[count].TDP.T4 = 0.0F;
				GPatch.WGs[count].TDP.L4 = 0.0F;

				break;
				
			case 3:
				//********************** TDPs *********************
				// Pitch: Levels in semitones 
				GPatch.WGs[count].TDP.T1 = 0.0F;
				GPatch.WGs[count].TDP.L1 = 0.0F;
				GPatch.WGs[count].TDP.T2 = 0.0F;
				GPatch.WGs[count].TDP.L2 = 0.0F;
				GPatch.WGs[count].TDP.T3 = 0.0F;
				GPatch.WGs[count].TDP.Sustain = 0.0F;
				GPatch.WGs[count].TDP.T4 = 0.0F;
				GPatch.WGs[count].TDP.L4 = 0.0F;
				break;
				
		}


#endif

#ifdef ENABLE_TDA

		switch (count)
		{
			case 0:
				// ********************* TDAs *********************
				// Amplitude: Levels in gain 0.0F to 1.0F
				GPatch.WGs[count].TDA.T1 = 0.8F;
				GPatch.WGs[count].TDA.L1 = 0.8F;
				GPatch.WGs[count].TDA.T2 = 1.0F;
				GPatch.WGs[count].TDA.L2 = 0.9F;
				GPatch.WGs[count].TDA.T3 = 1.0F;
				GPatch.WGs[count].TDA.Sustain = 1.0F;
				GPatch.WGs[count].TDA.T4 = 1.0F;
				GPatch.WGs[count].TDA.L4 = 0.0F;

				break;

			case 1:
				// ********************* TDAs *********************
				// Amplitude: Levels in gain 0.0F to 1.0F
				GPatch.WGs[count].TDA.T1 = 0.8F;
				GPatch.WGs[count].TDA.L1 = 0.8F;
				GPatch.WGs[count].TDA.T2 = 1.0F;
				GPatch.WGs[count].TDA.L2 = 0.9F;
				GPatch.WGs[count].TDA.T3 = 1.0F;
				GPatch.WGs[count].TDA.Sustain = 1.0F;
				GPatch.WGs[count].TDA.T4 = 1.0F;
				GPatch.WGs[count].TDA.L4 = 0.0F;
				break;

			case 2:
				// ********************* TDAs *********************
				// Amplitude: Levels in gain 0.0F to 1.0F
				GPatch.WGs[count].TDA.T1 = 0.8F;
				GPatch.WGs[count].TDA.L1 = 0.8F;
				GPatch.WGs[count].TDA.T2 = 1.0F;
				GPatch.WGs[count].TDA.L2 = 0.9F;
				GPatch.WGs[count].TDA.T3 = 1.0F;
				GPatch.WGs[count].TDA.Sustain = 1.0F;
				GPatch.WGs[count].TDA.T4 = 1.0F;
				GPatch.WGs[count].TDA.L4 = 0.0F;
				break;

			case 3:
				// ********************* TDAs *********************
				// Amplitude: Levels in gain 0.0F to 1.0F
				GPatch.WGs[count].TDA.T1 = 0.8F;
				GPatch.WGs[count].TDA.L1 = 0.8F;
				GPatch.WGs[count].TDA.T2 = 1.0F;
				GPatch.WGs[count].TDA.L2 = 0.9F;
				GPatch.WGs[count].TDA.T3 = 1.0F;
				GPatch.WGs[count].TDA.Sustain = 1.0F;
				GPatch.WGs[count].TDA.T4 = 1.0F;
				GPatch.WGs[count].TDA.L4 = 0.0F;
				break;
		}
#endif

	}

	// **************** Filter *******************
	GPatch.Filter.FilterType = filtertype::BPF;
	GPatch.Filter.Q = 20;
	GPatch.Filter.RelativeFrequency = 20.0F;
	GPatch.Filter.DBGain = 0.0F;

#ifdef ENABLE_TDF
		// ********************* TDFs *********************
		// Filter
		GPatch.TDF.T1 = 2.0F;
		GPatch.TDF.L1 = -40.0F;
		GPatch.TDF.T2 = 0.0F;
		GPatch.TDF.L2 = 0.0F;
		GPatch.TDF.T3 = 0.0F;
		GPatch.TDF.Sustain = -40.0F;
		GPatch.TDF.T4 = 2.0F;
		GPatch.TDF.L4 = 0.0F;
#endif


	GPatch.WGMixMode = WGMIX::MIX;

	// Reverb
	GPatch.Reverb.feedback = 0.60F;
	GPatch.Reverb.sampleLength = 3000;
	GPatch.Reverb.level = 0.2F;
	GPatch.Reverb.enabled = false;
}
