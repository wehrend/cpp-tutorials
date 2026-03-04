 setup:
 	# Download the MinGW development package
	wget https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.tar.gz

	# Extract it
	tar -xzf SDL2_ttf-devel-2.22.0-mingw.tar.gz

	# 1. Copy the headers and libs into your central SDL2 directory in the parent folder
	cp -r SDL2_ttf-2.22.0/x86_64-w64-mingw32/* ../SDL2-2.30.1/x86_64-w64-mingw32/

	# 2. Copy the DLLs to the parent folder so Wine finds them
	cp SDL2_ttf-2.22.0/x86_64-w64-mingw32/bin/*.dll ..
	
build:
	x86_64-w64-mingw32-g++ main.cpp Envelope.h Filter.h Oscillator.cpp -o synth.exe \
	-I../SDL2-2.30.1/x86_64-w64-mingw32/include/SDL2 \
	-L../SDL2-2.30.1/x86_64-w64-mingw32/lib \
	-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf \
	-mwindows \
	-static-libgcc -static-libstdc++ \
	-lwinmm -lole32 -lgdi32 -limm32 -loleaut32 -lversion -lsetupapi -luser32 -lkernel32

up: 
	WINEPATH=".." wine ./synth.exe