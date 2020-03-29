#include "Game.h"
#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "RenderingSystem.h"
#include "AudioSystem.h"
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

	AudioSystem::GetInstance().SetCallback(UpdateStateSyncWithAudio);
	AudioSystem::GetInstance().StartPlayback();
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
	//static double elapsedTime;

	//Timer::GetInstance().Tick();
	//double deltaTime = Timer::GetInstance().GetDeltaTime();

	//if (elapsedTime < 1.0 / 60.0)
	//	elapsedTime += deltaTime;
	//else
	//{
	//	while (!nes.FrameComplete())
	//		nes.Clock();

	//	std::cout << "fps: " << (int)(1.0 / elapsedTime) << std::endl;

	//	elapsedTime = 0.0;
	//}
}

void Game::UpdateStateSyncWithAudio()
{
	double cyclesPerBuffer = 5.369318 * 1000000 * 512 / 48000;
	double cyclesPerSample = cyclesPerBuffer / 512;

	double cyclesDone = 0.0;

	for (int i = 0; i < cyclesPerBuffer; i++)
	{
		GetInstance().nes.Clock();

		cyclesDone += 1.0;

		if (cyclesDone >= cyclesPerSample)
		{
			AudioSystem::GetInstance().EnqueueSample(GetInstance().nes.GetAudioSample());
			cyclesDone -= cyclesPerSample;
		}
	}
}

void Game::Render()
{
	SDL_SetRenderDrawColor(GraphicsSystem::GetInstance().GetRenderer(), 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderClear(GraphicsSystem::GetInstance().GetRenderer());

	RenderingSystem::GetInstance().Render(nes.GetScreen());

	SDL_RenderPresent(GraphicsSystem::GetInstance().GetRenderer());
}



