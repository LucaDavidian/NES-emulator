#include "controller.hpp"
#include "SDL.h"

Controller::Controller() : state{}
{
	keyboardState = SDL_GetKeyboardState(0);
}

bool Controller::GetState()
{
	bool button = state & 0x80;
	state <<= 1;

	return button;
}

void Controller::PollButtons()
{
	SDL_PumpEvents();

	if (keyboardState[SDL_SCANCODE_W])
		PressButton(Button::UP);
	else
		ReleaseButton(Button::UP);

	if (keyboardState[SDL_SCANCODE_S])
		PressButton(Button::DOWN);
	else
		ReleaseButton(Button::DOWN);

	if (keyboardState[SDL_SCANCODE_A])
		PressButton(Button::LEFT);
	else
		ReleaseButton(Button::LEFT);

	if (keyboardState[SDL_SCANCODE_D])
		PressButton(Button::RIGHT);
	else
		ReleaseButton(Button::RIGHT);

	if (keyboardState[SDL_SCANCODE_U])
		PressButton(Button::SELECT);
	else
		ReleaseButton(Button::SELECT);

	if (keyboardState[SDL_SCANCODE_I])
		PressButton(Button::START);
	else
		ReleaseButton(Button::START);

	if (keyboardState[SDL_SCANCODE_J])
		PressButton(Button::A);
	else
		ReleaseButton(Button::A);

	if (keyboardState[SDL_SCANCODE_K])
		PressButton(Button::B);
	else
		ReleaseButton(Button::B);
}