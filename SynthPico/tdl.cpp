#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>

#include "signal.h"
#include "constants.h"
#include "patch.h"
#include "keyPressedInfo.h"
#include "tdl.h"


const int TDL_RECALC_NUM = 512;

float* pVolummeMapping;

tdl::tdl()
{

}

tdl::~tdl()
{

}



void tdl::init(int wgID, tdlType type)
{
	_wgID = wgID;
	_type = type;
}



void InitVolumnMapping()
{
	pVolummeMapping = new float[VOLUME_MAPPING_STEPS];

	float input = 0.0F;
	float output;
	float step = 1.0F / VOLUME_MAPPING_STEPS;

	for (int count = 0; count < VOLUME_MAPPING_STEPS; count++)
	{
		output = powf(step * count, 2.0F);
		pVolummeMapping[count] = output;
	}
}
