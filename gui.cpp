#include "gui.h"

#include <SDL2/SDL.h>
#include "sequence.h"

void Knob::Draw(SDL_Renderer* renderer)
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

GUI::GUI(Sequence& seq)
	: m_sequence(seq)
{
	int i = 0;
	for (auto& step : seq.steps) {
		++i;
		m_knobs.emplace_back(i * 30 + 10, 120, &step.carrierFreq, 0.1f);
		m_knobs.emplace_back(i * 30 + 10, 180, &step.modFreq);
		m_knobs.emplace_back(i * 30 + 10, 220, &step.modDepth, 3.0f);
	}
}

void GUI::OnMouseDown(int x, int y)
{
	for (Knob& knob : m_knobs) {
		if (knob.InBounds(x, y)) {
			m_activeKnob = &knob;
			m_dragStart = y;
			return;
		}
	}
}

void GUI::OnMouseUp()
{
	m_activeKnob = nullptr;
	m_dragStart = 0;
}

void GUI::OnMouseMove(int x, int y)
{
	if (m_activeKnob) {
		*m_activeKnob->value -= (m_dragStart - y) / m_activeKnob->valueScaling;
		m_dragStart = y;

		if (*m_activeKnob->value < 0) 
			*m_activeKnob->value = 0;
	}
}

void GUI::Repaint(SDL_Renderer* renderer, int currentStep)
{
	SDL_Rect rect = {
		40,
		40,
		32,
		32
	};

	for (int i = 0; i < 8; i++) {
		SDL_SetRenderDrawColor(renderer, m_sequence.steps[i].modDepth * 8, m_sequence.steps[i].modFreq * 2, m_sequence.steps[i].carrierFreq / 3, 255);

		if (i == currentStep)
			SDL_RenderFillRect(renderer, &rect);
		else
			SDL_RenderDrawRect(renderer, &rect);

		rect.x += rect.w + 8;
	}

	SDL_SetRenderDrawColor(renderer, 127, 127, 255, 255);

	for (Knob& knob : m_knobs) {
		knob.Draw(renderer);
	}
}