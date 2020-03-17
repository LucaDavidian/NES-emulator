#include "Game.h"
#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "SDL.h"
#include "Timer.h"
#include <iostream>

Game &Game::GetInstance()
{
	static Game instance;

	return instance;
}

Game::~Game()
{
	SDL_Quit();
}

void Game::Initialize(int windowWidth, int windowHeight)
{
	GraphicsSystem::GetInstance().Initialize(windowWidth, windowHeight);

	nes.Initialize();

	Timer::GetInstance().Reset();
	Timer::GetInstance().Start();
}

void Game::Run()
{
	SDL_Event event;

	while (!mQuit)
	{
		// process input events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				mQuit = true;
			else
				InputSystem::GetInstance().ProcessInput(&event);
		}

		// perform game loop
		Loop();
	}
}

void Game::Loop()
{
	GetInput();
	UpdateState();
	Render();
}

void Game::GetInput()
{

}

void Game::UpdateState()
{	
	while (!nes.FrameComplete())	
		nes.Clock();

	Timer::GetInstance().Tick();
	double delta = Timer::GetInstance().GetDeltaTime();

	std::cout << "delta time: " << delta * 1000 << " ms" << std::endl;
	std::cout << "fps: " << (int)(1.0 / delta) << std::endl;
}

#include "RenderingSystem.h"

void Game::Render()
{
	SDL_SetRenderDrawColor(GraphicsSystem::GetInstance().GetRenderer(), 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderClear(GraphicsSystem::GetInstance().GetRenderer());

	RenderingSystem::GetInstance().Render(nes.GetScreen());

	SDL_RenderPresent(GraphicsSystem::GetInstance().GetRenderer());
}



