#include <SDL2/SDL.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

const float AMPLITUDE = 0.2f;
const int SAMPLE_RATE = 44100;

const float sequence[8] = { 130.81f, 220.f, 130.81f, 440.f, 330.f, 440.f, 130.81f, 261.63f };

struct CallbackInfo {
	int sampleCount;
	int currentStep;
};

void AudioCallback(void *userData, Uint8 *rawBuffer, int bytes)
{
	float *buffer = (float*)rawBuffer;
	int length = bytes / sizeof(float);
	CallbackInfo *info = (CallbackInfo*)userData;

	for (int i = 0; i < length; i++, info->sampleCount++) {
		float freq = sequence[info->currentStep];
		float time = (float)info->sampleCount / (float)SAMPLE_RATE;
		buffer[i] = (sinf(2.0f * M_PI * freq * time)) * AMPLITUDE;
	}
}

void DrawInterface(SDL_Renderer* renderer, int currentStep)
{
	SDL_Rect rect = { 
		(640 - 8 * (32 + 8)) / 2, 
		(320 - 32) / 2, 
		32, 
		32 
	};

	for (int i = 0; i < 8; i++) {
		SDL_SetRenderDrawColor(renderer, 128, i * (255 / 8), 255, 255);

		if (i == currentStep) {
			SDL_RenderFillRect(renderer, &rect);
		} else {
			SDL_RenderDrawRect(renderer, &rect);
		}

		rect.x += rect.w + 8;
	}
}

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cerr << "Failed to initalize SDL! " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("fmseq", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;

	CallbackInfo info;
	info.sampleCount = 0;
	info.currentStep = 0;

	SDL_AudioSpec desiredSpec;
	desiredSpec.freq = SAMPLE_RATE;
	desiredSpec.format = AUDIO_F32SYS;
	desiredSpec.channels = 1;
	desiredSpec.samples = 2048;
	desiredSpec.callback = AudioCallback;
	desiredSpec.userdata = &info;

	SDL_AudioSpec obtainedSpec;
	if (SDL_OpenAudio(&desiredSpec, &obtainedSpec) < 0) {
		std::cerr << "Failed to initalize audio! " << SDL_GetError() << std::endl;
		return 1;
	}

	if (desiredSpec.format != obtainedSpec.format) {
		std::cerr << "Different format: " << obtainedSpec.format << std::endl;
	}

	SDL_PauseAudio(0);

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		info.currentStep = (info.currentStep + 1) % 8;
		DrawInterface(renderer, info.currentStep);

		SDL_RenderPresent(renderer);
		SDL_UpdateWindowSurface(window);

		SDL_Delay(200);
	}

	SDL_PauseAudio(1);
	SDL_CloseAudio();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}