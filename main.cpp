#include <SDL2/SDL.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>

const float AMPLITUDE = 0.2f;
float sequence[8] = { 130.81f, 220.f, 130.81f, 440.f, 330.f, 440.f, 130.81f, 261.63f };

class Knob {
public:
	Knob(int x, int y, float *valuePtr)
		: m_x(x), m_y(y), value(valuePtr) {}

	void Draw(SDL_Renderer*);
	bool InBounds(int x, int y);

	float *value;
	float valueScaling = 0.3f;
private:
	int m_x, m_y;
};

std::vector<Knob> knobs;

class Window {
public:
	Window();
private:
	bool m_isRunning{ false };
};

void Knob::Draw(SDL_Renderer *renderer)
{
	constexpr int Radius = 25;
	SDL_Rect rect = { m_x, m_y, Radius, Radius };
	SDL_RenderDrawRect(renderer, &rect);

	SDL_RenderDrawLine(renderer, m_x, m_y + (*value * valueScaling), m_x + Radius, m_y + (*value * valueScaling));
}

bool Knob::InBounds(int x, int y)
{
	constexpr int Radius = 25;
	SDL_Rect rect = { m_x, m_y, Radius, Radius };
	SDL_Point point = { x, y };

	return SDL_PointInRect(&point, &rect);
}

struct CallbackInfo {
	int samplesDone;
	int samplesPerStep;
	int currentStep;
	int sampleRate;
};

void AudioCallback(void *userData, Uint8 *rawBuffer, int bytes)
{
	float *buffer = (float*)rawBuffer;
	int length = bytes / sizeof(float);
	CallbackInfo *info = (CallbackInfo*)userData;

	for (int i = 0; i < length; i++, info->samplesDone++) {
		float freq = sequence[info->currentStep];
		float time = (float)info->samplesDone / (float)info->sampleRate;

		if (info->samplesDone >= info->samplesPerStep) {
			info->samplesDone = 0;
			info->currentStep = (info->currentStep + 1) % 8;
		}

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

	for (Knob& knob : knobs) {
		knob.Draw(renderer);
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

	SDL_AudioSpec desiredSpec;
	desiredSpec.freq = 44100;
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

	if (desiredSpec.format != obtainedSpec.format)
		std::cerr << "Different format: " << obtainedSpec.format << std::endl;

	info.samplesDone = 0;
	info.samplesPerStep = obtainedSpec.freq * 0.4f;
	info.currentStep = 0;
	info.sampleRate = obtainedSpec.freq;

	SDL_PauseAudio(0);

	knobs.emplace_back(10, 120, sequence);
	knobs.emplace_back(40, 120, sequence + 1);
	knobs.emplace_back(70, 120, sequence + 2);
	knobs.emplace_back(110, 120, sequence + 3);
	knobs.emplace_back(140, 120, sequence + 4);
	knobs.emplace_back(170, 120, sequence + 5);
	knobs.emplace_back(210, 120, sequence + 6);
	knobs.emplace_back(240, 120, sequence + 7);

	bool running = true;
	Knob* mouseKnob = nullptr;
	int yStart = 0;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				for (Knob& knob : knobs) {
					if (knob.InBounds(event.button.x, event.button.y)) {
						mouseKnob = &knob;
						yStart = event.button.y;
						break;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if (mouseKnob) {
					*mouseKnob->value -= (yStart - event.button.y);
					yStart = event.button.y;
					if (*mouseKnob->value < 0) *mouseKnob->value = 0;
				}
				
				break;
			case SDL_MOUSEBUTTONUP:
				mouseKnob = nullptr;
				yStart = 0;
				break;
			}
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		DrawInterface(renderer, info.currentStep);

		SDL_RenderPresent(renderer);
		SDL_UpdateWindowSurface(window);

		SDL_Delay(10);
	}

	SDL_PauseAudio(1);
	SDL_CloseAudio();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}