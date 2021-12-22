#pragma once

struct StepData {
	float carrierFreq = 440.f;
	float modFreq = 0;
	float modDepth = 1;
};

class Sequence {
public:
	StepData steps[8];
};