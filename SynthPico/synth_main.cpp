/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>

#if PICO_ON_DEVICE

#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"

#endif

#include "constants.h"
#include "voice.h"
#include "patch.h"
#include "LCRFilter.h"
#include "wavegenerator.h"
#include "tdl.h"
#include "lfo.h"
#include "reverb.h"
#include "synth.h"

#include "pico/stdlib.h"

#if USE_AUDIO_I2S

#include "pico/audio_i2s.h"

#elif USE_AUDIO_PWM
#include "pico/audio_pwm.h"
#elif USE_AUDIO_SPDIF
#include "pico/audio_spdif.h"
#endif

#define SAMPLES_PER_BUFFER (128)

float GDeltaT = 1.0F / (float) SAMPLE_FREQUENCY; // Time between samples

synth GSynth;
patch GPatch;

float percent;
const int CONSOLE_BUFFER_SIZE = 64;
char consoleLine[CONSOLE_BUFFER_SIZE]; // Input buffer for commands
uint8_t consoleLineWritePos = 0;

struct audio_buffer_pool *init_audio() {

    static audio_format_t audio_format = {
        #if USE_AUDIO_SPDIF
            .sample_freq = SAMPLE_FREQUENCY,
        #else
            .sample_freq = SAMPLE_FREQUENCY,
        #endif
            .format = AUDIO_BUFFER_FORMAT_PCM_S16,
            .channel_count = 1,
    };

    static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = 2
    };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3,
                                                                      SAMPLES_PER_BUFFER); // todo correct size
    bool __unused ok;
    const struct audio_format *output_format;
#if USE_AUDIO_I2S
    struct audio_i2s_config config = {
            .data_pin = PICO_AUDIO_I2S_DATA_PIN,
            .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
            .dma_channel = 0,
            .pio_sm = 0,
    };

    output_format = audio_i2s_setup(&audio_format, &config);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }

    ok = audio_i2s_connect(producer_pool);
    assert(ok);
    audio_i2s_set_enabled(true);
#elif USE_AUDIO_PWM
    output_format = audio_pwm_setup(&audio_format, -1, &default_mono_channel_config);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }
    ok = audio_pwm_default_connect(producer_pool, false);
    assert(ok);
    audio_pwm_set_enabled(true);
#elif USE_AUDIO_SPDIF
    output_format = audio_spdif_setup(&audio_format, &audio_spdif_default_config);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }
    //ok = audio_spdif_connect(producer_pool);
    ok = audio_spdif_connect(producer_pool);
    assert(ok);
    audio_spdif_set_enabled(true);
#endif
    return producer_pool;
}


void SocketCommand(const string& com)
{
	int keyNo = -1;

	printf("%s\n",com.c_str()); // Print the command(s)

	// Single characters are easy play notes from keyboard		 

	if (com.size() == 1)
	{
		switch(com[0])
		{
			case 'a':
				keyNo = 60;
				break;

			case 's':
				keyNo = 62;
				break;

			case 'd':
				keyNo = 64;
				break;

			case 'f':
				keyNo = 65;
				break;

			case 'g':
				keyNo = 67;
				break;

			case 'h':
				keyNo = 69;
				break;

			case 'j':
				keyNo = 71;
				break;

			case 'k':
				keyNo = 72;
				break;

			case 'l':
				keyNo = 0;
				break;

			case '%':
				printf("Percent load %.2f%\n", percent);
				break;
		}

		// press the new one
		if (keyNo > -1)
		{
			GSynth.keyPressed(keyNo, 64);
		}
	}

	if (com.size() > 0)
	{
		// Split at : to get command name
		string findchar = ":";

		vector<string> splits = split(com, findchar);

		if (splits.size() == 2)
		{
			bool commandFound = false;

			if (splits[0] == "Patch")
			{
				commandFound = true;
				ProcessPatchLine(splits[1]);
			}

			if (splits[0] == "KeyPressed")
			{
				commandFound = true;
				int key = atoi(splits[1].c_str());
				GSynth.keyPressed(key, 64);
			}

			if (splits[0] == "KeyReleased")
			{
				commandFound = true;
				int key = atoi(splits[1].c_str());
				GSynth.keyReleased(key);
			}

			if (!commandFound)
			{
				fprintf(stderr, "unknown command %s\n", splits[0].c_str());
			}
		}
	}
}




int main() {
#if PICO_ON_DEVICE
#if USE_AUDIO_PWM
    set_sys_clock_48mhz();
#endif
#endif

    stdio_init_all();

    struct audio_buffer_pool *ap = init_audio();

    GSynth.InitalisePatches();

    while (true) {
        int c = getchar_timeout_us(0);
        if (c >= 0) {

            if (c == '\n' || c == '\r')
            {
                // New line, end of input
                consoleLine[consoleLineWritePos++] = 0; // Terminate the string
                consoleLineWritePos = 0;

                printf("command = %s \n", consoleLine);

                SocketCommand(consoleLine);
            }
            else
            {
                consoleLine[consoleLineWritePos++] = c; // Append to console read bufer

                if (consoleLineWritePos >= CONSOLE_BUFFER_SIZE)
                {
                    consoleLineWritePos = 0; // Read buffer overrun, reset
                }
            }
        }
        struct audio_buffer *buffer = take_audio_buffer(ap, true);

        int16_t *samples = (int16_t *) buffer->buffer->bytes;
        int16_t sample;

        for (uint i = 0; i < buffer->max_sample_count; i++) {
            sample = GSynth.getnext(GDeltaT).left.ToIntLimit();
            samples[i] = sample;
        }
        buffer->sample_count = buffer->max_sample_count;
        give_audio_buffer(ap, buffer);

    }
    puts("\n");
    return 0;
}
