
APP = ising.exe

SDL2_PATH = C:\libraries\SDL2-2.0.5\x86_64-w64-mingw32

CFLAGS = -std=c99 -O3 -I$(SDL2_PATH)\include
LFLAGS = -L$(SDL2_PATH)\lib -lmingw32 -lSDL2main -lSDL2

build: main

main: main.c
	gcc -o $(APP) main.c $(CFLAGS) $(LFLAGS)

run: main
	./$(APP)

clean: 
	rm -rf $(APP)