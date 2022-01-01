#include "gui.h"

#include <SDL2/SDL.h>
#include "sequence.h"

void Slider::Draw(SDL_Renderer* renderer)
{
	SDL_RenderDrawRect(renderer, &m_bounds);
	SDL_RenderDrawLine(renderer, m_bounds.x, m_bounds.y + (*value * valueScaling), m_bounds.x + m_bounds.w, m_bounds.y + (*value * valueScaling));
}

bool Slider::InBounds(int x, int y)
{
	SDL_Point point = { x, y };
	return SDL_PointInRect(&point, &m_bounds);
}

GUI::GUI(Sequence& seq)
	: m_sequence(seq)
{
	int i = 0;
	for (auto& step : seq.steps) {
		m_sliders.emplace_back(i * 40 + 14, 60, &step.carrierFreq, 0.1f);
		m_sliders.emplace_back(i * 40 + 14, 145, &step.modFreq);
		m_sliders.emplace_back(i * 40 + 14, 230, &step.modDepth, 3.0f);
		i++;
	}
}

void GUI::OnMouseDown(int x, int y)
{
	for (Slider& slider : m_sliders) {
		if (slider.InBounds(x, y)) {
			m_activeSlider = &slider;
			m_dragStart = y;
			return;
		}
	}
}

void GUI::OnMouseUp()
{
	m_activeSlider = nullptr;
	m_dragStart = 0;
}

void GUI::OnMouseMove(int x, int y)
{
	if (m_activeSlider) {
		*m_activeSlider->value -= (m_dragStart - y) / m_activeSlider->valueScaling;
		m_dragStart = y;

		if (*m_activeSlider->value < 0)
			*m_activeSlider->value = 0;
	}
}

void GUI::Repaint(SDL_Renderer* renderer, int currentStep)
{
	SDL_Rect rect = { 10, 10, 32, 32 };

	for (int i = 0; i < 8; i++) {
		SDL_SetRenderDrawColor(renderer, m_sequence.steps[i].modDepth * 8, m_sequence.steps[i].modFreq * 2, m_sequence.steps[i].carrierFreq / 3, 255);

		if (i == currentStep)
			SDL_RenderFillRect(renderer, &rect);
		else
			SDL_RenderDrawRect(renderer, &rect);

		rect.x += rect.w + 8;
	}

	SDL_SetRenderDrawColor(renderer, 127, 127, 255, 255);

	for (Slider& slider : m_sliders) {
		slider.Draw(renderer);
	}
}