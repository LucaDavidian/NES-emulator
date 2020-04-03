#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>

class Controller
{
public:
	enum struct Button { RIGHT, LEFT, DOWN, UP, START, SELECT, B, A };
public:
	Controller();

	void PressButton(Button button) { state |= 1 << static_cast<int>(button); }
	void ReleaseButton(Button button) { state &= ~(1 << static_cast<int>(button)); }
	void PollButtons();
	bool GetState();
private:
	uint8_t state;
	const uint8_t *keyboardState;
};

#endif  // CONTROLLER_H
