#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "SDL.h"

class GraphicsSystem
{
public:
	static GraphicsSystem &GetInstance();

	~GraphicsSystem();

	void Initialize(int windowWidth, int windowHeight);

	SDL_Renderer *GetRenderer() const { return mRenderer; }
	SDL_Window *GetWindow() const { return mWindow; }

	int GetWindowWidth() const { return mWindowWidth; }
	int GetWindowHeight() const { return mWindowHeight; }
private:
	GraphicsSystem() = default;

	SDL_Window *mWindow;
	SDL_Renderer *mRenderer;

	int mWindowWidth;
	int mWindowHeight;
};

#endif  // GRAPHICS_SYSTEM_H
