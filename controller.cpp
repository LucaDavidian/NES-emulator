#include "controller.h"
#include "SDL.h"

Controller::Controller() : state{}
{
	keyboardState = SDL_GetKeyboardState(0);
}

bool Controller::GetState()
{
	bool button = state.reg & 0x80;
	state.reg <<= 1;

	return button;
}

void Controller::PollButtons()
{
	SDL_PumpEvents();

	if (keyboardState[SDL_SCANCODE_W])
		state.bits.up = 1;
	else
		state.bits.up = 0;

	if (keyboardState[SDL_SCANCODE_S])
		state.bits.down = 1;
	else
		state.bits.down = 0;

	if (keyboardState[SDL_SCANCODE_A])
		state.bits.left = 1;
	else
		state.bits.left = 0;

	if (keyboardState[SDL_SCANCODE_D])
		state.bits.right = 1;
	else
		state.bits.right = 0;

	if (keyboardState[SDL_SCANCODE_U])
		state.bits.select = 1;
	else
		state.bits.select = 0;

	if (keyboardState[SDL_SCANCODE_I])
		state.bits.start = 1;
	else
		state.bits.start = 0;

	if (keyboardState[SDL_SCANCODE_J])
		state.bits.A = 1;
	else
		state.bits.A = 0;

	if (keyboardState[SDL_SCANCODE_K])
		state.bits.B = 1;
	else
		state.bits.B = 0;
}