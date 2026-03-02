install_depends:
	cd ~/development/cpp-tutorials
	wget https://github.com/libsdl-org/SDL/releases/download/release-2.30.1/SDL2-devel-2.30.1-mingw.tar.gz
	tar -xzvf SDL2-devel-2.30.1-mingw.tar.gz
	cd SDL2-2.30.1
	make install-package CROSS_PATH=/usr/x86_64-w64-mingw32

build:
	x86_64-w64-mingw32-g++ main.cpp -o hello_tone.exe \
    -I./SDL2-2.30.1/x86_64-w64-mingw32/include/SDL2 \
    -L./SDL2-2.30.1/x86_64-w64-mingw32/lib \
    -static-libgcc -static-libstdc++ \
    -Wl,-Bstatic -lSDL2main -lSDL2 -Wl,-Bdynamic \
    -lwinmm -lole32 -lgdi32 -limm32 -loleaut32 -lversion -lsetupapi -luser32 -lkernel32