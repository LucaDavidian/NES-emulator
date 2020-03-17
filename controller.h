#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>

class Controller
{
public:
	enum struct Button { A, B, SELECT, START, UP, DOWN, LEFT, RIGHT };
public:
	Controller();

	void PressButton(Button button) { state.reg |= 1 << static_cast<int>(button); }
	void ReleaseButton(Button button) { state.reg &= ~(1 << static_cast<int>(button)); }
	void PollButtons();
	bool GetState();
private:
	union
	{
		struct
		{
			uint8_t right : 1;
			uint8_t left : 1;
			uint8_t down : 1;
			uint8_t up : 1;
			uint8_t start : 1;
			uint8_t select : 1;
			uint8_t B : 1;
			uint8_t A : 1;
		} bits;
		uint8_t reg;
	} state;
	const uint8_t *keyboardState;
};

#endif  // CONTROLLER_H
