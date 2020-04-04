#include "Game.h"
#include "GraphicsSystem.h"
//#include "InputSystem.h"
#include "RenderingSystem.h"
#include "AudioSystem.h"
#include "Timer.h"
#include <iostream>
#include "SDL.h""

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

	Timer::GetInstance().Reset();
	Timer::GetInstance().Start();

	// initialize finite state machine
	playGameState.game = this;
	PushState(&menuGameState);
}

void Game::Run()
{
	SDL_Event event;

	while (true)
	{
		// process input events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
				switch (event.key.keysym.sym)
				{
					case SDLK_m:
						if (stateStack.Top() != &menuGameState)
							PopState();
						break;
					case SDLK_p:
							if (stateStack.Top() != &playGameState)
						PushState(&playGameState);
						break;
					case SDLK_ESCAPE:
						while (!stateStack.Empty())
							PopState();
						mQuit = true;
						break;
				}
		}

		if (mQuit)
			break;

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

/**** game state machine ****/
void Game::ChangeState(GameState *state)
{
	if (!stateStack.Empty())
	{
		stateStack.Top()->Exit();
		stateStack.Pop();
	}
	
	state->Enter();
	stateStack.Push(state);
}

void Game::PushState(GameState *state)
{
	state->Enter();
	stateStack.Push(state);
}

void Game::PopState()
{
	stateStack.Top()->Exit();
	stateStack.Pop();
}

// menu state
void MenuGameState::GetInput()
{

}

void MenuGameState::UpdateState()
{

}

void MenuGameState::Render()
{
	SDL_SetRenderDrawColor(GraphicsSystem::GetInstance().GetRenderer(), 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderClear(GraphicsSystem::GetInstance().GetRenderer());



	SDL_RenderPresent(GraphicsSystem::GetInstance().GetRenderer());
}

// play state
Game *PlayGameState::game = nullptr;

void PlayGameState::Enter()
{
	game->nes.Initialize();

	AudioSystem::GetInstance().SetCallback(UpdateStateSyncWithAudio);
	AudioSystem::GetInstance().StartPlayback();
}

void PlayGameState::Exit()
{
	AudioSystem::GetInstance().StopPlayback();
}

void PlayGameState::GetInput()
{

}

void PlayGameState::UpdateState()
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
}

void PlayGameState::Render()
{
	SDL_SetRenderDrawColor(GraphicsSystem::GetInstance().GetRenderer(), 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderClear(GraphicsSystem::GetInstance().GetRenderer());

	RenderingSystem::GetInstance().Render(game->nes.GetScreen());

	SDL_RenderPresent(GraphicsSystem::GetInstance().GetRenderer());
}

void PlayGameState::UpdateStateSyncWithAudio()
{
	double cyclesPerSample = 5.369318 * 1000000 / 44100;  // PPU frequency == 5.369318 MHz
	double cyclesPerBuffer = cyclesPerSample * 512;

	double cyclesDone = 0.0;

	for (int i = 0; i < cyclesPerBuffer; i++)
	{
		game->nes.Clock();

		cyclesDone += 1.0;

		if (cyclesDone >= cyclesPerSample)
		{
			AudioSystem::GetInstance().EnqueueSample(game->nes.GetAudioSample());
			cyclesDone -= cyclesPerSample;
		}
	}
}
