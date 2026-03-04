build:
	x86_64-w64-mingw32-g++ main.cpp Envelope.h Filter.h Oscillator.cpp -o synth.exe \
	-I../SDL2-2.30.1/x86_64-w64-mingw32/include/SDL2 \
	-L../SDL2-2.30.1/x86_64-w64-mingw32/lib \
	-lmingw32 -lSDL2main -lSDL2 \
	-mwindows \
	-static-libgcc -static-libstdc++ \
	-lwinmm -lole32 -lgdi32 -limm32 -loleaut32 -lversion -lsetupapi -luser32 -lkernel32

up: 
	WINEPATH=".." wine ./synth.exe