
ifeq ($(OS), Windows_NT)
	# Windows environment
	# assumes that SDL2 is installed in a custom location
	SDL2_PATH = C:\libraries\SDL2-2.0.5\x86_64-w64-mingw32

	APP = ising.exe
	XCFLAGS = -I$(SDL2_PATH)\include
	XLFLAGS = -L$(SDL2_PATH)\lib -lmingw32
else
	# Linux environment
	# assumes that SDL2 is installed in /usr/include & /usr/lib
	APP = ising
	XCFLAGS =
	XLFLAGS = -lm
endif

CFLAGS = -std=c99 -O3 $(XCFLAGS)
LFLAGS = -lSDL2main -lSDL2 $(XLFLAGS)

build: main

main: main.c
	gcc -o $(APP) main.c $(CFLAGS) $(LFLAGS)

run: main
	./$(APP)

clean:
	rm -rf $(APP)
