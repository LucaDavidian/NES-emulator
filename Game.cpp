#include "Game.h"
#include "GraphicsSystem.h"
#include "RenderingSystem.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "Timer.h"
#include "Error.h"

MenuGameState Game::menuGameState;
PlayGameState Game::playGameState;

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
	PushState(&menuGameState);
}

void Game::Run()
{
	SDL_Event event;

	while (true)
	{
		// process input events (SDL event pump)
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
				switch (event.key.keysym.sym)
				{
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
	ProcessInput();
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

#include <filesystem>
#include "SDL_ttf.h"
#include "SDL_image.h"

// menu state
MenuGameState::~MenuGameState()
{
	TTF_Quit();
	IMG_Quit();
}

MenuGameState::Texture MenuGameState::CreateTextTexture(TTF_Font *font, std::string const &text, SDL_Color color)
{
	Texture texture;

	int textWidth;
	int textHeight;
	TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

	SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), color);

	if (!textSurface)
		Error("can't create text surface", SDL_GetError());

	texture.texture = SDL_CreateTextureFromSurface(GraphicsSystem::GetInstance().GetRenderer(), textSurface);

	if (!texture.texture)
		Error("can't create text texture", SDL_GetError());

	texture.textureRect = SDL_Rect{ 10, 10, textWidth, textHeight };

	return texture;
}

void MenuGameState::Enter()
{
	// C++ STL filesystem library
	namespace fs = std::filesystem;

	fs::path romPath("./ROMs");

	for (const fs::directory_entry &romFile : fs::directory_iterator(romPath))
	{
		fs::path gamePath(romFile.path().filename());

		if (gamePath.has_extension())
			romNames.InsertLast(gamePath.stem().string());
	}

	// sort rom names
	auto sort = [](Vector <std::string> &v)
	{
		for (int i = 0; i < v.Size() - 1; i++)
			for (int j = i + 1; j < v.Size(); j++)
				if (v[i] > v[j])
					std::swap(v[i], v[j]);
	};

	sort(romNames);

	// initialize SDL TTF library
	if (TTF_Init())
		Error("can't initialize TTF lib", TTF_GetError());

	// title
	TTF_Font *titleFont = TTF_OpenFont("fonts/8-Bit Madness.ttf", 50);

	if (!titleFont)
		Error("can't open font", TTF_GetError());

	SDL_Color textColor = { 255, 255, 255, 255 };

	title = CreateTextTexture(titleFont, "NES Emulator", textColor);

	TTF_CloseFont(titleFont);

	// rom names
	TTF_Font *nameFont = TTF_OpenFont("fonts/8-Bit Madness.ttf", 20);

	if (!nameFont)
		Error("can't open font", TTF_GetError());

	for (int i = 0; i < romNames.Size(); i++)
		romNameTextures.InsertLast(CreateTextTexture(nameFont, romNames[i], textColor));

	// info panel
	infoPanel.InsertLast(CreateTextTexture(nameFont, "UP/DOWN: select ROM, RETURN: play", textColor));
	infoPanel.InsertLast(CreateTextTexture(nameFont, "M: menu, ESC: quit", textColor));
	infoPanel.InsertLast(CreateTextTexture(nameFont, "NES buttons: WASD - direction, U/I - select/start, J/K - A/B", textColor));

	infoPanel[0].textureRect.y = 50;
	infoPanel[1].textureRect.y = infoPanel[0].textureRect.y + infoPanel[0].textureRect.h;
	infoPanel[2].textureRect.y = infoPanel[1].textureRect.y + infoPanel[1].textureRect.h;

	TTF_CloseFont(nameFont);

	// initialize SDL IMAGE library 
	if (!IMG_Init(IMG_INIT_PNG))
		Error("can't initialize SDL IMAGE library", IMG_GetError());

	// arrow
	SDL_Surface *arrowSurface = IMG_Load("res/arrow.png");

	if (!arrowSurface)
		Error("can't create image surface", SDL_GetError());

	arrow.texture = SDL_CreateTextureFromSurface(GraphicsSystem::GetInstance().GetRenderer(), arrowSurface);

	if (!arrow.texture)
		Error("can't create text texture", SDL_GetError());

	int textureWidth;
	int textureHeight;
	SDL_QueryTexture(arrow.texture, 0, 0, &textureWidth, &textureHeight);

	arrow.textureRect.w = textureWidth;
	arrow.textureRect.h = textureHeight;

	arrow.textureRect.h -= 30;
	arrow.textureRect.w -= 30;
}

void MenuGameState::Exit()
{
	SDL_DestroyTexture(title.texture);

	for (Texture &texture : romNameTextures)
		SDL_DestroyTexture(texture.texture);

	SDL_DestroyTexture(arrow.texture);
}

void MenuGameState::ProcessInput()
{
	InputSystem::GetInstance().PollKeyboard();  // input system polling

	if (selected < romNames.Size() - 1 && InputSystem::GetInstance().GetKeyState(SDL_SCANCODE_DOWN) == InputSystem::KeyState::JUST_PRESSED)
		selected += 1;
	else if (selected > 0 && InputSystem::GetInstance().GetKeyState(SDL_SCANCODE_UP) == InputSystem::KeyState::JUST_PRESSED)
		selected -= 1;

	if (InputSystem::GetInstance().GetKeyState(SDL_SCANCODE_RETURN) == InputSystem::KeyState::JUST_PRESSED)
	{
		std::string romPath("ROMs/");
		romPath += romNames[selected] + ".nes";
		Game::GetInstance().nes.InsertCartridge(romPath.c_str());

		Game::GetInstance().PushState(&Game::playGameState);
	}
}

void MenuGameState::UpdateState()
{
	int row = 120;

	for (int i = 0; i < romNameTextures.Size(); i++)
	{
		romNameTextures[i].textureRect.y = row;
		row += romNameTextures[i].textureRect.h;
	}

	arrow.textureRect.x = 10 + romNameTextures[selected].textureRect.x + romNameTextures[selected].textureRect.w;
	arrow.textureRect.y = romNameTextures[selected].textureRect.y - (arrow.textureRect.h - romNameTextures[selected].textureRect.h) / 2;
}

void MenuGameState::Render()
{
	SDL_SetRenderDrawColor(GraphicsSystem::GetInstance().GetRenderer(), 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderClear(GraphicsSystem::GetInstance().GetRenderer());

	SDL_RenderCopy(GraphicsSystem::GetInstance().GetRenderer(), title.texture, 0, &title.textureRect);

	for (Texture &texture : romNameTextures)
		SDL_RenderCopy(GraphicsSystem::GetInstance().GetRenderer(), texture.texture, 0, &texture.textureRect);

	SDL_RenderCopy(GraphicsSystem::GetInstance().GetRenderer(), arrow.texture, 0, &arrow.textureRect);

	for (Texture &texture : infoPanel)
		SDL_RenderCopy(GraphicsSystem::GetInstance().GetRenderer(), texture.texture, 0, &texture.textureRect);

	SDL_RenderPresent(GraphicsSystem::GetInstance().GetRenderer());
}

// play state
void PlayGameState::Enter()
{
	screenTexture = SDL_CreateTexture(GraphicsSystem::GetInstance().GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);

	if (!screenTexture)
		Error("can't create texture", SDL_GetError());

	Game::GetInstance().nes.Reset();

	AudioSystem::GetInstance().SetCallback(UpdateStateSyncWithAudio);
	AudioSystem::GetInstance().StartPlayback();
}

void PlayGameState::Exit()
{
	AudioSystem::GetInstance().StopPlayback();

	SDL_DestroyTexture(screenTexture);
	screenTexture = nullptr;

	Game::GetInstance().nes.RemoveCartridge();
}

void PlayGameState::ProcessInput()
{
	InputSystem::GetInstance().PollKeyboard();  // input system polling

	if (InputSystem::GetInstance().GetKeyState(SDL_SCANCODE_M) == InputSystem::KeyState::JUST_PRESSED)
		Game::GetInstance().PopState();
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

	auto screenBuffer = Game::GetInstance().nes.GetScreen().GetBuffer();
	//uint8_t const (&screenBuffer)[240][256][3] = screen.GetBuffer();

	void *pixels;
	int pitch;

	SDL_LockTexture(screenTexture, 0, &pixels, &pitch);

	for (int i = 0; i < 240; i++)
		for (int j = 0; j < 256; j++)
			((uint32_t*)pixels)[i * 256 + j] = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888), screenBuffer[i][j][0], screenBuffer[i][j][1], screenBuffer[i][j][2]);

	SDL_UnlockTexture(screenTexture);

	SDL_Rect dstRect = { 0, 0, 256 * 2, 240 * 2 };
	SDL_RenderCopy(GraphicsSystem::GetInstance().GetRenderer(), screenTexture, 0, &dstRect);

	SDL_RenderPresent(GraphicsSystem::GetInstance().GetRenderer());
}

void PlayGameState::UpdateStateSyncWithAudio()
{
	double cyclesPerSample = 5.369318 * 1000000 / 44100;  // PPU frequency == 5.369318 MHz
	double cyclesPerBuffer = cyclesPerSample * 512;

	double cyclesDone = 0.0;

	for (int i = 0; i < cyclesPerBuffer; i++)
	{
		Game::GetInstance().nes.Clock();

		cyclesDone += 1.0;

		if (cyclesDone >= cyclesPerSample)
		{
			AudioSystem::GetInstance().EnqueueSample(Game::GetInstance().nes.GetAudioSample());
			cyclesDone -= cyclesPerSample;
		}
	}
}
