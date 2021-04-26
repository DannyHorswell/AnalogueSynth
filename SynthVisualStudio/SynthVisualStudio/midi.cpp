#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <signal.h>
#include <math.h>

#ifdef __arm__
#include <alsa/asoundlib.h>
#endif

#include "constants.h"
#include "midi.h"

#ifdef ENABLE_MIDI

const unsigned char CONTROL_MASK = 0x80;

/*NoteOn noteOnCallback;
NoteOff noteOffCallback;
AfterTouch afterTouchCallback;
// TODO Control mode
ProgramChange programChangeCallback;

PitchBend pitchBendCallback;*/


//void midi::startmidi(void* ptr)
void midi::startmidi(char* device_in)
{
	fprintf(stderr,"midi::startmidi\n");
	//char *device_in = (char*) ptr;

	int i;
    int err;

    if (device_in)
	{
		err = snd_rawmidi_open(&handle_in,NULL,device_in,SND_RAWMIDI_NONBLOCK);    
            
		if (err)
		{
			fprintf(stderr,"snd_rawmidi_open %s failed: %d\n",device_in,err);
		}

		if (!handle_in)
		{
			fprintf(stderr,"snd_rawmidi_open no handle returned\n");
		}
    }

	/*if (handle_in)
	{
		 // Enable non-blocking mode
	    snd_rawmidi_nonblock(handle_in, 1);  
	}*/

	return;
}

void midi::pollmidi()
{
	size_t ret;

	do
	{
		ret = snd_rawmidi_read(handle_in,&ch,1); 
		

		if (ret != -EAGAIN)
		{
			if (ch & CONTROL_MASK)
			{
				bytecount = 0;

				data[bytecount++] = ch;

				lastMessageType = (messageType) (ch & 0xF0);

				// Extract the control type and channel if relevent
				if ((int) lastMessageType == 0xF0)
				{
					lastMessageType = (messageType) (ch & 0xFF);
					channel = 0;
				}
				else
				{
					channel = ch & 0x0F;
				}
			}
			else
			{
				if (lastMessageType != messageType::UNDEFINED)
				{
					if (bytecount < MIDI_DATA_BUFFER_SIZE)
					{
						// Capture the data byte
						data[bytecount++] = ch;
					}
					else
					{
						// too many bytes, ignore
					}
				}
			}

			if (bytecount == 1)
			{
				// single byte message
				switch (lastMessageType)
				{
					case messageType::UNDEFINED:
						// Ignore, we were not expecting data
						bytecount = 0;
						break;

						// ************ Unknown messages
					case messageType::UNDEFINED1:
					case messageType::UNDEFINED2:
					case messageType::UNDEFINED3:
					case messageType::UNDEFINED4:

						bytecount = 0;
						// Ignore
						lastMessageType = messageType::UNDEFINED;
						break;

					// ************  O Data bytes
					case messageType::SYSTEM_RESET:
					case messageType::ACTIVE_SENSING:
					case messageType::START:
					case messageType::CONTINIE:
					case messageType::STOP:
					case messageType::TIMING_CLOCK:
					case messageType::END_SYSEX:
					case messageType::TUNE_REQUEST:
					
						// Handle zero byte commands
						lastMessageType = messageType::UNDEFINED;
						break;

					default:
						// Ignore, waiting on more bytes
						break;
				}
			}

			if (bytecount == 2)
			{
				// 2 byte messages
				switch (lastMessageType)
				{
					case messageType::UNDEFINED:
						// Ignore, we were not expecting data
						bytecount = 0;
						break;

					// ************ 1 Data byte
					case messageType::SONG_SELECT:
					case messageType::CHAN_AFTER_TOUCH:

						lastMessageType = messageType::UNDEFINED;
						break;

					case messageType::PROGRAM_CHANGE:
						if (programChangeCallback)
						{
							programChangeCallback(channel, data[1]);
						}

						lastMessageType = messageType::UNDEFINED;

						break;

					default:
						// Ignore, waiting on more bytes
						break;
				}
			}

			if (bytecount == 3)
			{

				// 3 byte messages
				switch (lastMessageType)
				{
					case messageType::UNDEFINED:
						// Ignore, we were not expecting data
						bytecount = 0;
						break;

					// ************ 2 Data bytes
					case messageType::PITCH_WHEEL:

						if (pitchBendCallback)
						{
							// 16384 = mid position
							pitchBendCallback(channel, ((unsigned int) data[1]) + ((unsigned int) data[2]) << 8);
						}

						lastMessageType = messageType::UNDEFINED;
						break;

					case messageType::CONTROL_CHANGE:
						if (controlChangeCallback)
						{
							controlChangeCallback(channel, (controlChangeType) data[1] ,((unsigned int) data[2]) );
						}
						break;
						
					case messageType::SONG_POS:

						// Ignore
						lastMessageType = messageType::UNDEFINED;
						break;

					case messageType::POLY_AFTER_TOUCH:

						if (afterTouchCallback)
						{
							afterTouchCallback(channel, data[1], data[2]);
						}

						lastMessageType = messageType::UNDEFINED;
						break;

					case messageType::NOTE_OFF:

						if (noteOffCallback)
						{
							noteOffCallback(channel, data[1], data[2]);
						}

						lastMessageType = messageType::UNDEFINED;
						break;

					case messageType::NOTE_ON:
						fprintf(stderr,"Note ON Note %d, velocity %d\n", data[1], data[2]);
						if (noteOnCallback)
						{
							noteOnCallback(channel, data[1], data[2]);
						}

						lastMessageType = messageType::UNDEFINED;
						break;

					default:
						// Ignore, waiting on more bytes
						break;
				}
			}

			if (bytecount > 3)
			{
				// Variable length messages
				switch (lastMessageType)
				{
					case messageType::UNDEFINED:
					
					case messageType::SYSEXC:
					case messageType::MIDI_TIME_QTR_FRAME:
					default:
						// Ignore
						bytecount = 0;
						lastMessageType = messageType::UNDEFINED;

						printf("%x\n", lastMessageType);
						break;
				}
			}
		}
	}
	while (ret != -EAGAIN);
}

void midi::stopmidi()
{
	snd_rawmidi_drain(handle_in); 
	snd_rawmidi_close(handle_in);
}

// Converts control change 0 - 7F to 1/max to max log value
float controlChangeValueToLog(int cc, float max)
{
	float minToMax = (cc - 0x3F) ;
	float minus1To1 = minToMax / 64.0F;
	float logVal = pow(2.0, minus1To1 * log2(max));
	float ret = logVal;


	printf("%f\n", logVal);
	return ret;
}

#endif
