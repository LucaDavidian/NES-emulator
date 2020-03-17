#include "RenderingSystem.h"
#include "GraphicsSystem.h"
#include "screen.hpp"
#include "Error.h"

RenderingSystem &RenderingSystem::GetInstance()
{
	static RenderingSystem instance;

	return instance;
}

RenderingSystem::RenderingSystem() : texture(nullptr)
{
	texture = SDL_CreateTexture(GraphicsSystem::GetInstance().GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
	
	if (!texture)
		Error("can't create texture", SDL_GetError());
}

void RenderingSystem::Render(Screen &screen)
{
	auto screenBuffer = screen.GetBuffer();
	//uint8_t const (&screenBuffer)[240][256][3] = screen.GetBuffer();

	void *pixels;
	int pitch;

	SDL_LockTexture(texture, 0, &pixels, &pitch);

	for (int i = 0; i < 240; i++)
		for (int j = 0; j < 256; j++)
			((uint32_t*)pixels)[i * 256 + j] = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888), screenBuffer[i][j][0], screenBuffer[i][j][1], screenBuffer[i][j][2]);

	SDL_UnlockTexture(texture);

	SDL_Rect dstRect = { 0, 0, 256 * 2, 240 * 2 };
	SDL_RenderCopy(GraphicsSystem::GetInstance().GetRenderer(), texture, 0, &dstRect);
}