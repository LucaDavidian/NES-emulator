#ifndef GAME_H
#define GAME_H 

#include "NES.hpp"
#include "../../../data structures/ADT/stack/stack.hpp"
#include "../../../data structures/vector/vector.hpp"
#include "SDL.h"
#include <string>

class Game;
typedef struct _TTF_Font TTF_Font;
struct SDL_Texture;

/**** game states ****/
class GameState
{
public:
	GameState() = default;
	virtual ~GameState() = default;

	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void ProcessInput() = 0;
	virtual void UpdateState() = 0;
	virtual void Render() = 0;
};

class MenuGameState : public GameState
{
public:
	MenuGameState() = default;
	~MenuGameState();

	void Enter() override;
	void Exit() override;
	void ProcessInput() override;
	void UpdateState() override;
	void Render() override;
private:
	struct Texture
	{
		SDL_Texture *texture;
		SDL_Rect textureRect;
	};
	Texture CreateTextTexture(TTF_Font *font, std::string const &text, SDL_Color color);
	Vector<std::string> romNames;
	Texture title;
	Vector<Texture> romNameTextures;
	Texture arrow;
	Vector<Texture> infoPanel;
	uint8_t selected = 0;
};

class PlayGameState : public GameState
{
	friend class Game;
public:
	PlayGameState() : screenTexture(nullptr) {}
	~PlayGameState() = default;

	void Enter() override;
	void Exit() override;
	void ProcessInput() override;
	void UpdateState() override;
	void Render() override;
private:
	static void UpdateStateSyncWithAudio();
	SDL_Texture *screenTexture;
};

class Game
{
friend class MenuGameState;
friend class PlayGameState;
public:
	static Game &GetInstance();

	~Game();

	void Initialize(int windowWidth, int windowHeight);
	void Run();
private:
	Game() = default;

	void Loop();
	void ProcessInput() { stateStack.Top()->ProcessInput(); }
	void UpdateState() { stateStack.Top()->UpdateState(); }
	void Render() { stateStack.Top()->Render(); }

	NES nes;

	/**** game FSM ****/
	static MenuGameState menuGameState;
	static PlayGameState playGameState;

	void ChangeState(GameState *state);
	void PushState(GameState *state);
	void PopState();

	Stack<GameState*> stateStack;
	bool mQuit = false;
};

#endif  // GAME_H