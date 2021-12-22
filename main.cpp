#include <SDL2/SDL.h>
#include <iostream>

#include "audio.h"
#include "gui.h"
#include "sequence.h"

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cerr << "Failed to initalize SDL! " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("fmseq", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;

	Sequence sequence;
	AudioContext audio(sequence);
	GUI gui(sequence);

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				gui.OnMouseDown(event.button.x, event.button.y);
				break;
			case SDL_MOUSEMOTION:
				gui.OnMouseMove(event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				gui.OnMouseUp();
				break;
			}
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		gui.Repaint(renderer, audio.CurrentStep());

		SDL_RenderPresent(renderer);
		SDL_UpdateWindowSurface(window);

		SDL_Delay(10);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}