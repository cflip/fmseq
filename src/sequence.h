#pragma once

struct StepData {
	float carrierFreq = 110.f;
	float modFreq = 0;
	float modDepth = 1;
};

class Sequence {
public:
	StepData steps[8];
};