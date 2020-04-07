#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include "SDL.h"
//#include "../../../data structures/ADT/queue/queue.hpp"

class Controller;

class InputSystem
{
public:
	enum class KeyState { JUST_PRESSED, JUST_RELEASED, PRESSED, RELEASED };
public:
	static InputSystem &GetInstance(); 

	//void RegisterController(Controller *controller) { this->controller = controller; }
	
	void PollKeyboard();
	KeyState GetKeyState(SDL_Scancode key);
private:
	InputSystem();
	//Controller *controller;

	uint8_t oldKeyboardState[256];
	uint8_t keyboardState[256];

	//Queue<InpuEvent> inputEvents;
};

#endif  // INPUT_SYSTEM_H
