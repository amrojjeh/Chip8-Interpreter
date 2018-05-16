#include <stdio.h>

#include "Chip8.h"

int startingTime;
bool running = true;

void LimitFPS(int Fps)
{
	if ((1000 / Fps) > SDL_GetTicks() - startingTime) // 1000 / Fps = Converting Fps to Frames per millisecond
	{
		SDL_Delay(1000/Fps - (SDL_GetTicks() - startingTime));
	}
}


int main(int argc, char* argv[])
{
	if (argc < 2) // No rom has been given
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing ROM", "No ROM has been loaded.", 0);
		return 1;
	}
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	// Creating the window
	SDL_Window* window =  SDL_CreateWindow("Chip8 Emulator", 
											SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
											WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
	if (window == NULL)
	{
		printf("ERROR! %s", SDL_GetError());
		running = false;
	}
	
	SDL_Surface* windowSurface = SDL_GetWindowSurface(window);
	
	if (windowSurface == 0)
	{
		printf("ERROR! %s", SDL_GetError());
		running = false;
	}
	
	SDL_Event event;
	Chip8 Chip(windowSurface);
	
	if (Chip.loadFile(argv[1]) == -1) // If loading file failed
	{
		SDL_Log("File could not be loaded...");
		running = false;
	}
	while(running)
	{
		while(SDL_PollEvent(&event))
		{
			startingTime = SDL_GetTicks();
			switch (event.type)
			{
				case SDL_QUIT:
					running = false;
					break;
			}
		}
		Chip.emulateCycle();
		SDL_UpdateWindowSurface(window);
		LimitFPS(150);
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}