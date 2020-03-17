#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include "SDL.h"

class Controller;

class InputSystem
{
public:
	static InputSystem &GetInstance(); 

	void RegisterController(Controller *controller) { this->controller = controller; }

	void ProcessInput(SDL_Event *input);
	void PollInput();
private:
	InputSystem() = default;
	Controller *controller;
};

#endif  // INPUT_SYSTEM_H
