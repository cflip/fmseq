#pragma once

#include <SDL2/SDL.h>

#include <vector>
#include "sequence.h"

class Slider {
public:
	Slider(int x, int y, float* valuePtr, float scaling = 1.f)
		: m_bounds({ x, y, 25, 75 }), value(valuePtr), valueScaling(scaling) {}

	void Draw(SDL_Renderer*);
	bool InBounds(int x, int y);

	float* value;
	float valueScaling;
private:
	SDL_Rect m_bounds;
};

class GUI {
public:
	GUI(Sequence&);

	void OnMouseDown(int x, int y);
	void OnMouseUp();
	void OnMouseMove(int x, int y);

	void Repaint(SDL_Renderer*, int currentStep);
private:
	std::vector<Slider> m_sliders;

	Sequence& m_sequence;
	Slider* m_activeSlider{ nullptr };
	int m_dragStart{ 0 };
};