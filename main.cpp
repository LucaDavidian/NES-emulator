#include "Game.h"

const int NESDisplayWidth = 256;
const int NESDisplayHeight = 240;

int main(int argc, char *argv[])
{
	Game::GetInstance().Initialize(NESDisplayWidth * 2, NESDisplayHeight * 2);
	Game::GetInstance().Run();

	return 0;
}