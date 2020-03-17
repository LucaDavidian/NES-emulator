#include "InputSystem.h"
#include "Controller.h"

InputSystem &InputSystem::GetInstance()
{
	InputSystem instance;

	return instance;
}

void InputSystem::ProcessInput(SDL_Event *inputEvent)
{
	switch (inputEvent->type)
	{
		case SDL_KEYDOWN:
			switch (inputEvent->key.keysym.sym)
			{
				case SDLK_q:
					SDL_PushEvent(new SDL_Event{ SDL_QUIT });  // event copied to event queue (leak!)
					break;
				//case SDLK_w:
				//	if (controller)
				//		controller->PressButton(Controller::Button::UP);
				//	break;
				//case SDLK_s:
				//	if (controller)
				//		controller->PressButton(Controller::Button::DOWN);
				//	break;
				//case SDLK_a:
				//	if (controller)
				//		controller->PressButton(Controller::Button::LEFT);
				//	break;
				//case SDLK_d:
				//	if (controller)
				//		controller->PressButton(Controller::Button::RIGHT);
				//	break;
			}
			break;
		case SDL_KEYUP:
			switch (inputEvent->key.keysym.sym)
			{
			//case SDLK_w:
			//	if (controller)
			//		controller->ReleaseButton(Controller::Button::UP);
			//	break;
			//case SDLK_s:
			//	if (controller)
			//		controller->ReleaseButton(Controller::Button::DOWN);
			//	break;
			//case SDLK_a:
			//	if (controller)
			//		controller->ReleaseButton(Controller::Button::LEFT);
			//	break;
			//case SDLK_d:
			//	if (controller)
			//		controller->ReleaseButton(Controller::Button::RIGHT);
			//	break;
			}
			break;
	}
}

void InputSystem::PollInput()
{

}