#include <iostream>

#include "Constants.h"
#include "PortAudio.h"

PaError PortAudio::Initalise(PaStreamCallback* pBufferCallback)
{
    PaError paError;

    paError = Pa_Initialize();

    if (paError != paNoError)
    {
        red_consolecolour
            printf("PortAudio error: %s\n", Pa_GetErrorText(paError));
        reset_consolecolour
            return paError;
    }

    int deviceCount = Pa_GetDeviceCount();

    int defaultOutputDevice = Pa_GetDefaultOutputDevice();

#ifdef __arm__
    //system("clear");
#else
    system("CLS");
#endif

    const PaDeviceInfo* info;

    for (int count = 0; count < deviceCount; count++)
    {
        info = Pa_GetDeviceInfo(count);

        printf("Device: %s, ID: %d NOutChanels: %d \n", info->name, count, info->maxOutputChannels);
    }

    PaStreamParameters outParams;

    // TODO - Read from command args or config file
    outParams.device = 2;
    outParams.sampleFormat = paInt16;
#ifdef __arm__
    // Temp for raspberry pi
    outParams.device = Pa_GetDefaultOutputDevice();
    outParams.sampleFormat = paFloat32;
#endif


    outParams.channelCount = 1;

    outParams.hostApiSpecificStreamInfo = NULL;
    outParams.suggestedLatency = Pa_GetDeviceInfo(outParams.device)->defaultHighOutputLatency;

    printf("Openning device: %d, Sample rate %d\n", outParams.device, SAMPLE_FREQUENCY);
    paError = Pa_OpenStream(&paStream, NULL, &outParams, SAMPLE_FREQUENCY, FRAMES_PER_BUFFER, paClipOff, pBufferCallback, NULL);

    if (paError != paNoError)
    {
        red_consolecolour
            printf("PortAudio error: %s\n", Pa_GetErrorText(paError));
        reset_consolecolour
            return paError;
    }

    paError = Pa_StartStream(paStream);

    if (paError != paNoError)
    {
        red_consolecolour
            printf("PortAudio error: %s\n", Pa_GetErrorText(paError));
        reset_consolecolour
            return paError;
    }

    return paNoError;
}


PaError PortAudio::Stop()
{

    PaError paError = Pa_StopStream(paStream);

    if (paError != paNoError)
    {
        red_consolecolour
            printf("PortAudio error: %s\n", Pa_GetErrorText(paError));
        reset_consolecolour
            return paError;
    }

    paError = Pa_CloseStream(paStream);

    if (paError != paNoError)
    {
        red_consolecolour
            printf("PortAudio error: %s\n", Pa_GetErrorText(paError));
        reset_consolecolour
            return paError;
    }

    Pa_Terminate();

    return paNoError;
}

