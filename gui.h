#pragma once

#include <vector>
#include "sequence.h"

struct SDL_Renderer;

class Knob {
public:
	Knob(int x, int y, float* valuePtr, float scaling = 1.f)
		: m_x(x), m_y(y), value(valuePtr), valueScaling(scaling) {}

	void Draw(SDL_Renderer*);
	bool InBounds(int x, int y);

	float* value;
	float valueScaling;
private:
	int m_x, m_y;
};

class GUI {
public:
	GUI(Sequence&);

	void OnMouseDown(int x, int y);
	void OnMouseUp();
	void OnMouseMove(int x, int y);

	void Repaint(SDL_Renderer*, int currentStep);
private:
	std::vector<Knob> m_knobs;
	Knob* m_activeKnob{ nullptr };
	int m_dragStart{ 0 };
};