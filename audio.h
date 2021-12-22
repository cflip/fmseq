#pragma once

class Sequence;

struct CallbackInfo {
	int samplesDone;
	int samplesPerStep;
	int currentStep;
	int sampleRate;
	Sequence* seq;
};

class AudioContext {
public:
	AudioContext(Sequence&);
	~AudioContext();

	void Start();
	void Stop();

	int CurrentStep();
private:
	CallbackInfo m_info;
};