/*
 * Monte Carlo Ising Model simulator
 *
 * Project in course Programming in C and Assembly
 *
 * -----------------------------------------------
 * Description:
 * The program simulates the spin orientations of atoms in a ferromagnetic (J>0) or anti-ferromagnetic (J<0) material.
 * Temperature and external applied magnetic field are adjustible. At lower temperatures (T<2.0) and no external
 * magnetic field, the spins form domains and at higher temperatures, the spins are erratic (this illustrates the
 * Curie temperature). Hysteresis effect can also be observed by applying a magnetic field in one direction, getting
 * all spins aligned and then trying to align it back with opposite magnetic field.
 * -----------------------------------------------
 *
 * SDL code was written with the help of http://lazyfoo.net/tutorials/SDL/
 *
 * Kristjan Eimre
 * 29.12.2015
 *
 */

#include <SDL2\SDL.h>

#include <stdbool.h>
#include <stdio.h>

#include <time.h> // for srand(time(NULL)) and rand()
#include <stdlib.h>

#include <math.h> // for pow()

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

// The resolution of the grid of atoms
int rows = 100;
int cols = 100;

float h = 0.0; // external uniform magnetic field
float t = 0.2; // temperature
float j = 1.0; // spin coupling

// wait time after each transition in ms
int wait = 0;

SDL_Window* gWindow = NULL; 	//The window we'll be rendering to
SDL_Renderer* gRenderer = NULL; 	//The window renderer


bool init();	//Starts up SDL and creates window
void close();	//Frees media and shuts down SDL

void renderState(int rows, int cols, char state[rows][cols]); 		//Draws the current spin state
bool transitionState(int rows, int cols, char state[rows][cols]);	//Makes a single spin transition


bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init( SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {

		//Create window
		gWindow = SDL_CreateWindow("Ising Model", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
				SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}

void renderState(int rows, int cols, char state[rows][cols]) {
	float dx = (float) SCREEN_WIDTH / cols;
	float dy = (float) SCREEN_HEIGHT / rows;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (state[i][j] == 1) {
				SDL_Rect fillRect = { dx * i, dy * j, dx + 0.5, dy + 0.5 };
				SDL_SetRenderDrawColor(gRenderer, 200, 200, 20, 0xFF);
				SDL_RenderFillRect(gRenderer, &fillRect);
			}
		}
	}

}

void close() {
//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

//Quit SDL subsystems
	SDL_Quit();
}

void initState(int rows, int cols, char state[rows][cols]) {
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			state[i][j] = 2 * (rand() % 2) - 1;
}

bool transitionState(int rows, int cols, char state[rows][cols]) {
	// choose a random position
	int x = rand() % cols;
	int y = rand() % rows;

	// left, right, top and bottom coordinates
	// NB: the boundary conditions are periodic!
	int xL = (x == 0) ? (cols - 1) : (x - 1);
	int xR = (x == cols - 1) ? (0) : (x + 1);
	int yT = (y == 0) ? (rows - 1) : (y - 1);
	int yB = (y == rows - 1) ? (0) : (y + 1);

	// initial state energy
	float eni = -j * state[x][y] * (state[xL][y] + state[xR][y] + state[x][yT] + state[x][yB]) - h * state[x][y];
	// final state energy (after flipping the spin)
	float enf = -eni;

	if (enf >= eni) {
		// The final energy is higher than initial, accept the transition with certain probability
		float prob = expf(-1 / t * (enf - eni));
		bool accept = rand() < RAND_MAX * prob;
		if (accept) {
			state[x][y] *= -1;
			return true;
		}
	} else {
		// The final energy is lower than initial, make transition
		state[x][y] *= -1;
		return true;
	}
	return false;
}

bool handleEvents(SDL_Event e) {
	while (SDL_PollEvent(&e) != 0) {
		//User requests quit
		if (e.type == SDL_QUIT) {
			return true;
		}
		//User presses a key
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				t += 0.1;
				printf("T = %.2f\n", t);
				break;
			case SDLK_DOWN:
				t -= 0.1;
				if (t < 0.0)
					t = 0.0;
				printf("T = %.2f\n", t);
				break;
			case SDLK_LEFT:
				h -= 0.05;
				printf("H = %.2f\n", h);
				break;
			case SDLK_RIGHT:
				h += 0.05;
				printf("H = %.2f\n", h);
				break;
			case SDLK_j:
				j += 0.1;
				printf("J = %.2f\n", j);
				break;
			case SDLK_n:
				j -= 0.1;
				printf("J = %.2f\n", j);
				break;
			case SDLK_k:
				wait += 10;
				printf("wait = %d\n", wait);
				break;
			case SDLK_m:
				wait -= 10;
				if (wait < 0)
					wait = 0;
				printf("wait = %d\n", wait);
				break;
			case SDLK_ESCAPE:
				return true;
			}
		}

	}
	return false;
}

void printHelp() {
	printf("Command line parameters: \n");
	printf("    main.exe <rows> <columns> \n");
	printf("    <rows> - atom resolution in y direction (default: 70) \n");
	printf("    <columns> - atom resolution in x direction (default: 70) \n");

	printf("Keyboard shortcuts: \n");
	printf("    UP - increases temperature \n");
	printf("    DOWN - decreases temperature \n");
	printf("    RIGHT - increases magnetic field \n");
	printf("    LEFT - decreases magnetic field \n");
	printf("    J - increases spin coupling \n");
	printf("    N - decreases spin coupling \n");
	printf("    K - increases wait time after transition (ms) \n");
	printf("    M - decreases wait time after transition (ms) \n");
	printf("    ESC - closes the program \n");

	printf("Initial parameters: \n");
	printf("    T = %.2f\n", t);
	printf("    H = %.2f\n", h);
	printf("    J = %.2f\n", j);
	printf("    wait = %d\n", wait);
	printf("\n");
}

int main(int argc, char* args[]) {

	// Parse command line arguments (resolution data)
	if (argc != 1)
		if (argc != 3) {
			printf("Incorrect command line arguments, using default values.\n");
		} else {
			rows = atoi(args[1]);
			cols = atoi(args[2]);
		}

	printHelp();

	srand(time(NULL));

	char state[rows][cols];
	initState(rows, cols, state);

	//Start up SDL and create window
	if (!init()) {
		printf("Failed to initialize!\n");
	} else {
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		// Start the timer to control simulation speed
		Uint32 timer = SDL_GetTicks();

		bool transitionReady = false;

		//While application is running
		while (!quit) {
			//Handle events on queue
			quit = handleEvents(e);

			// calculate the next transition
			if (!transitionReady) {
				transitionReady = transitionState(rows, cols, state);
			}

			// render only if transition has been found and enough time has passed
			if (transitionReady && SDL_GetTicks() - timer > wait) {
				timer = SDL_GetTicks();
				transitionReady = false;
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 20, 20, 140, 0xFF);
				SDL_RenderClear(gRenderer);

				renderState(rows, cols, state);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
