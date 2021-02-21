#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <alsa/asoundlib.h>
#include <signal.h>

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
		//fprintf(stderr,"After snd_rawmidi_read\n");

		if (ret != -EAGAIN)
		{
			if (ch & CONTROL_MASK)
			{
				bytecount = 0;

				data[bytecount++] = ch;

				lastControl = (control) (ch & 0xF0);

				// Extract the control type and channel if relevent
				if (lastControl == 0xF0)
				{
					lastControl = (control) (ch & 0xFF);
					channel = 0;
				}
				else
				{
					channel = ch & 0x0F;
				}
			}
			else
			{
				if (lastControl != UNDEFINED)
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
				switch (lastControl)
				{
					case UNDEFINED:
						// Ignore, we were not expecting data
						bytecount = 0;
						break;

						// ************ Unknown messages
					case UNDEFINED1:
					case UNDEFINED2:
					case UNDEFINED3:
					case UNDEFINED4:

						bytecount = 0;
						// Ignore
						lastControl = UNDEFINED;
						break;

					// ************  O Data bytes
					case SYSTEM_RESET:
					case ACTIVE_SENSING:
					case START:
					case CONTINIE:
					case STOP:
					case TIMING_CLOCK:
					case END_SYSEX:
					case TUNE_REQUEST:
					
						// Handle zero byte commands
						lastControl = UNDEFINED;
						break;

					default:
						// Ignore, waiting on more bytes
						break;
				}
			}

			if (bytecount == 2)
			{
				// 2 byte messages
				switch (lastControl)
				{
					case UNDEFINED:
						// Ignore, we were not expecting data
						bytecount = 0;
						break;

					// ************ 1 Data byte
					case SONG_SELECT:
					case CHAN_AFTER_TOUCH:

						lastControl = UNDEFINED;
						break;

					case PROGRAM_CHANGE:
						if (programChangeCallback)
						{
							programChangeCallback(channel, data[1]);
						}

						lastControl = UNDEFINED;

						break;

					default:
						// Ignore, waiting on more bytes
						break;
				}
			}

			if (bytecount == 3)
			{

				// 3 byte messages
				switch (lastControl)
				{
					case UNDEFINED:
						// Ignore, we were not expecting data
						bytecount = 0;
						break;

					// ************ 2 Data bytes
					case PITCH_WHEEL:

						if (pitchBendCallback)
						{
							// 16384 = mid position
							pitchBendCallback(channel, ((unsigned int) data[1]) + ((unsigned int) data[2]) << 8);
						}

						lastControl = UNDEFINED;
						break;
						
					case SONG_POS:

						// Ignore
						lastControl = UNDEFINED;
						break;

					case POLY_AFTER_TOUCH:

						if (afterTouchCallback)
						{
							afterTouchCallback(channel, data[1], data[2]);
						}

						lastControl = UNDEFINED;
						break;

					case NOTE_OFF:

						if (noteOffCallback)
						{
							noteOffCallback(channel, data[1], data[2]);
						}

						lastControl = UNDEFINED;
						break;

					case NOTE_ON:

						if (noteOnCallback)
						{
							noteOnCallback(channel, data[1], data[2]);
						}

						lastControl = UNDEFINED;
						break;

					default:
						// Ignore, waiting on more bytes
						break;
				}
			}

			if (bytecount > 3)
			{
				// Variable length messages
				switch (lastControl)
				{
					case UNDEFINED:
					case CONTROL_CHANGE:
					case SYSEXC:
					case MIDI_TIME_QTR_FRAME:
					default:
						// Ignore
						bytecount = 0;
						lastControl = UNDEFINED;
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

#endif
