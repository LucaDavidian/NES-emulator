#include "InputSystem.h"
#include "controller.hpp"

InputSystem &InputSystem::GetInstance()
{
	static InputSystem instance;

	return instance;
}

InputSystem::InputSystem()
{
	for (int i = 0; i < 256; i++)
		oldKeyboardState[i] = 0;
}

void InputSystem::PollKeyboard()
{
	// save old keyboard state
	for (int i = 0; i < 256; i++)
		oldKeyboardState[i] = keyboardState[i];

	// poll current keyboard state
	const uint8_t *newKeyboardState = SDL_GetKeyboardState(0);

	for (int i = 0; i < 256; i++)
		keyboardState[i] = newKeyboardState[i];
}

InputSystem::KeyState InputSystem::GetKeyState(SDL_Scancode key)
{
	if (keyboardState[key])
		if (oldKeyboardState[key])
			return KeyState::PRESSED;
		else
			return KeyState::JUST_PRESSED;
	else
		if (oldKeyboardState[key])
			return KeyState::JUST_RELEASED;
		else
			return KeyState::RELEASED;
}