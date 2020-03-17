#ifndef GAME_H
#define GAME_H 

#include "NES.h"

class Game
{
public:
	static Game &GetInstance();

	~Game();

	void Initialize(int windowWidth, int windowHeight);
	void Run();
private:
	Game() = default;

	void Loop();
	void GetInput();
	void UpdateState();
	void Render();

	bool mQuit = false;

	NES nes;
};

#endif  // GAME_H