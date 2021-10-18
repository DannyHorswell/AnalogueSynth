#pragma once

enum tdlType
{
	P = 0,
	A = 1,
	F = 2
};


void InitVolumnMapping();

class tdl
{
	tdlType _type;
	int _wgID; 

public:
	signal output;
	signal releaseLevel;
	signal pressLevel;
	bool lastKeystate;

	tdl();
	~tdl();

	void init(int wgID, tdlType type);

	inline signal getnext(keyPressedInfo kpi)
{
	patchTDL* ppatchTDL = NULL;

	switch (_type)
	{
#ifdef ENABLE_TDP
		case P:
			ppatchTDL = &GPatch.WGs[_wgID].TDP;
			break;
#endif

#ifdef ENABLE_TDA
		case A:
			ppatchTDL = &GPatch.WGs[_wgID].TDA;
			break;
#endif

#ifdef ENABLE_TDF
		case F:
			ppatchTDL = &GPatch.TDF;
			break;
#endif
	}
	if (ppatchTDL == NULL)
	{
		return 1.0F;
	}

	signal amountThrough;
	signal amountLeft;

	if (kpi.keyPressed)
	{
		if (lastKeystate == false)
		{
			pressLevel = output;
		}
		// In pressed phase

		// Is it between 0 and 1
		if (kpi.timeSincePressed <= ppatchTDL->T1)
		{
			amountThrough = kpi.timeSincePressed / ppatchTDL->T1;
			output = pressLevel +  (ppatchTDL->L1 -  pressLevel) * amountThrough;
		}

		// Is it between 1 and 2
		else if (kpi.timeSincePressed <= ppatchTDL->T1 + ppatchTDL->T2)
		{
			amountThrough = (kpi.timeSincePressed - ppatchTDL->T1) / ppatchTDL->T2;
			output = ppatchTDL->L1 + (ppatchTDL->L2 - ppatchTDL->L1) * amountThrough;
		}

		// Is it between 2 and 3
		else if (kpi.timeSincePressed <= ppatchTDL->T1 + ppatchTDL->T2 + ppatchTDL->T3)
		{
			amountThrough = (kpi.timeSincePressed - ppatchTDL->T1 - ppatchTDL->T2) / ppatchTDL->T3;
			output = ppatchTDL->L2 +  (ppatchTDL->Sustain - ppatchTDL->L2) * amountThrough;
		}
		else
 		{
			output = ppatchTDL->Sustain;
		}
		
		lastKeystate = true;
	}
	else
	{
		// In release phase
		if (lastKeystate)
		{
			// Key just release, record output level
			releaseLevel = output;
		}

		if (kpi.timeSinceReleased < ppatchTDL->T4)
		{
			amountLeft = (ppatchTDL->T4 - kpi.timeSinceReleased) / ppatchTDL->T4;
			amountThrough = sig_1 - amountLeft;

			output = ppatchTDL->L4 * amountThrough + releaseLevel * amountLeft;
		}
		else
		{
			output = ppatchTDL->L4;
		}
		
		lastKeystate = false;
	}

	/*if (_type == A)
	{
		// We want logarythmic for volume
		int index = (int) (output.ToFloat() * VOLUME_MAPPING_STEPS);

		// Limit to 0 - 1 range
		index < 0 ? index = 0 : index;
		index >= VOLUME_MAPPING_STEPS ? index = VOLUME_MAPPING_STEPS - 1 : index;

		output = pVolummeMapping[index];
	}*/

	return output;
}

	
};

