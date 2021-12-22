#include "audio.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>

#include "sequence.h"

static const float AMPLITUDE = 0.2f;

// SDL periodically calls this function to fill a chunk of the audio output buffer
static void AudioCallback(void* userData, Uint8* rawBuffer, int bytes)
{
	float* buffer = (float*)rawBuffer;
	int length = bytes / sizeof(float);
	CallbackInfo* info = (CallbackInfo*)userData;
	StepData& step = info->seq->steps[info->currentStep];

	for (int i = 0; i < length; i++, info->samplesDone++) {
		float time = (float)info->samplesDone / (float)info->sampleRate;
		float modWave = sinf(2.0f * M_PI * step.modFreq * time);
		float freq = step.carrierFreq + modWave * step.modDepth;

		// Check if enough samples have been generated for this step
		if (info->samplesDone >= info->samplesPerStep) {
			// If so, move on to the next step
			info->samplesDone = 0;
			step = info->seq->steps[info->currentStep];
			info->currentStep = (info->currentStep + 1) % 8;
		}

		buffer[i] = (sinf(2.0f * M_PI * freq * time)) * AMPLITUDE;
	}
}

AudioContext::AudioContext(Sequence& seq)
{
	SDL_AudioSpec desiredSpec;
	desiredSpec.freq = 44100;
	desiredSpec.format = AUDIO_F32SYS;
	desiredSpec.channels = 1;
	desiredSpec.samples = 2048;
	desiredSpec.callback = AudioCallback;
	desiredSpec.userdata = &m_info;

	SDL_AudioSpec obtainedSpec;
	if (SDL_OpenAudio(&desiredSpec, &obtainedSpec) < 0) {
		std::cerr << "Failed to initalize audio! " << SDL_GetError() << std::endl;
		std::exit(1);
	}

	// The audio callback expects float values, so if a different data type 
	// is obtained we should throw an error.
	if (desiredSpec.format != obtainedSpec.format) {
		std::cerr << "Unexpected audio format: " << obtainedSpec.format << std::endl;
		std::exit(1);
	}

	m_info.samplesDone = 0;
	m_info.samplesPerStep = obtainedSpec.freq * 0.4f;
	m_info.currentStep = 0;
	m_info.sampleRate = obtainedSpec.freq;
	m_info.seq = &seq;

	this->Start();
}

AudioContext::~AudioContext()
{
	this->Stop();
	SDL_CloseAudio();
}

void AudioContext::Start()
{
	SDL_PauseAudio(0);
}

void AudioContext::Stop()
{
	SDL_PauseAudio(1);
}

int AudioContext::CurrentStep()
{
	return m_info.currentStep;
}