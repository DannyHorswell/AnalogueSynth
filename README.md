# AnalogueSynth - Analogue synth emulator

Welcome to my analogue synth emulator.

In the 80s, I used to play around with analogue synthasizers. I had a Korg Detla, Roland alpha Juno, and a Roland D50, to play with when I was a teenager.

This project I created a few years back to try and recreate an analogue synth using a RPI 2. I had some success but found the RPI 2 was not man enough for
the job. So this was left sitting on a drive at home for some time. Now I recently purchased a RPI 400 which is much fater, so thought would be a good
project to see if I can make it work on that.

The initial version of this I wrote using ALSA. When ressurecting this project, I found alsa was very flaky (or maybe my code was) so I decided to change to use
PortAudio, which I have used for a cople of other projects. This seems to be working well now.

I have tried to make this cross platform. The main project is a visual studio solution, but to compile on a RPI there is a make file. I have been
compiling mainly on the RPI, and used visula studio to move some code I had on another project for changing from ALSA to PortAudio. 

Features

Multi voice (configurable, I have 12 set in constants.h)

Wave generators
	Multi wave generators per voice (configurable, I have 4 set in config file)
	Sin, Square (PWM LFO controlled), Saw, Triangle, Noise (Gaussian) and WAV sample wave generators
	Mix or ring wave generator mixing

Low frequency oscillators
	Sin wave
	Square wave
	Saw
	Random level square

Time dependent levels
	Can controll WG amplitude, frequencie, filter etc

Reverb
	Stereo cross over, adjustable delay and feedback etc


Patches
	Patches can be stored and loaded and chnaged on the fly	by console commands or from a net Socket
	(I did have a windows WPF app that could modify all vaues above on the fly, I need to find that and add here)

MIDI
	Note on (Velocity)
	Note off
	Control change
	Pitch bend

Filter
	LCR Resonant LP Filterfilter

MIDI Conrolled
	






Getting working

RPI.

You should be able to run 'make' in the visual studio folder containing the make file.
You will need to install PortAudio from http://portaudio.com/

MIDI - If you don't have a midi controlled, disable midi in the Constants.h file by commenting out this line #define ENABLE_MIDI
If you do have midi, you need to ensure this setting is correct for your environment in main.cpp file

static char mididevice[] = "hw:1,0";   

you can find teh correct midi device value bu running

amidi -l




