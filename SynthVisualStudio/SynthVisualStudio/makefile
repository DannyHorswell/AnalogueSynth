synth : synth.o wavegenerator.o main.o voice.o patch.o lfo.o reverb.o midi.o tdl.o LCRFilter.o \
wav.o socket.o benchmark.o noise.o sampleset.o portaudio.o
	g++ -lasound -pthread -o synth main.o synth.o wavegenerator.o voice.o patch.o lfo.o \
reverb.o tdl.o midi.o filter.o wav.o socket.o benchmark.o noise.o sampleset.o portaudio.o \
 -L libasound /usr/local/lib/libportaudio.so

main.o : main.cpp main.h constants.h synth.h patch.h
	g++ -c -O2 main.cpp

portaudio.o : portaudio.cpp portaudio.h constants.h
	g++ -c -O2 portaudio.cpp

LCRFilter.o : LCRFilter.cpp LCRFilter.h constants.h
	g++ -c -O2 LCRFilter.cpp 

patch.o : patch.cpp patch.h constants.h
	g++ -c -O2 patch.cpp

lfo.o : lfo.cpp lfo.h constants.h patch.h noise.h
	g++ -c -O2 lfo.cpp

synth.o : synth.cpp synth.h constants.h patch.h
	g++ -c -O2 synth.cpp

voice.o : voice.cpp voice.h constants.h patch.h
	g++ -c -O2 voice.cpp

wavegenerator.o : wavegenerator.cpp wavegenerator.h constants.h patch.h filter.h noise.h
	g++ -c -O2 wavegenerator.cpp

reverb.o : reverb.cpp reverb.h constants.h patch.h
	g++ -c -O2 reverb.cpp

tdl.o : tdl.cpp tdl.h constants.h patch.h
	g++ -c -O2 tdl.cpp

midi.o : midi.cpp midi.h constants.h patch.h
	g++ -c -O2 midi.cpp

wav.o : wav.cpp wav.h constants.h
	g++ -c -O2 wav.cpp

socket.o : socket.cpp socket.h constants.h
	g++ -c -O2 socket.cpp

benchmark.o : benchmark.cpp benchmark.h constants.h
	g++ -c -O2 benchmark.cpp

noise.o : noise.cpp noise.h constants.h
	g++ -c -O2 noise.cpp

sampleset.o : sampleset.cpp sampleset.h constants.h
	g++ -c -O2 sampleset.cpp

clean :
	rm synth *.o
 
		