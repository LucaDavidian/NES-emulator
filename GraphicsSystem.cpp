#include "GraphicsSystem.h"
#include "Error.h"

GraphicsSystem &GraphicsSystem::GetInstance()
{
	static GraphicsSystem instance;

	return instance;
}

GraphicsSystem::~GraphicsSystem()
{
	// free resources
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
}

void GraphicsSystem::Initialize(int windowWidth, int windowHeight)
{
	// init video (and event) subsystems
	if (SDL_Init(SDL_INIT_VIDEO) != 0)  
		Error("can't initialize video", SDL_GetError());

	// create window
	mWindow = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);

	if (!mWindow)
		Error("can't create window", SDL_GetError());

	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;

	// create renderer (default render target is the window from which it is created)
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);

	if (!mRenderer)
		Error("can't create renderer", SDL_GetError());
}
