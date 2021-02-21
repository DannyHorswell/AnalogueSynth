#pragma once
#ifdef _WIN32
#include <dsound.h>
#include <dsconf.h>
#include <pa_asio.h>
#include <portaudio.h>
#endif

#ifdef __arm__
#include "/home/pi/portaudio/include/portaudio.h"
//#include <pa_linux_alsa.h>
#endif

class PortAudio
{
private:
	PaStream* paStream;

public:

	PaError Initalise(PaStreamCallback* pBufferCallback);

	PaError Stop();
};

