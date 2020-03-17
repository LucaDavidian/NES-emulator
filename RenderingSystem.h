#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "SDL.h"

class Screen;

class RenderingSystem
{
public:
	static RenderingSystem &GetInstance();

	~RenderingSystem() { SDL_DestroyTexture(texture); }

	void Render(Screen &screen);
private:
	RenderingSystem();

	SDL_Texture *texture;
};

#endif  // RENDERING_SYSTEM_H
