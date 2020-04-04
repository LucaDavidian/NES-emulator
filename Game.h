#ifndef GAME_H
#define GAME_H 

#include "NES.hpp"
#include "../../../data structures/ADT/stack/stack.hpp"

class Game;

/**** game states ****/
class GameState
{
public:
	GameState() = default;
	virtual ~GameState() = default;

	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void GetInput() = 0;
	virtual void UpdateState() = 0;
	virtual void Render() = 0;
};

class MenuGameState : public GameState
{
public:
	MenuGameState() = default;
	~MenuGameState() = default;

	void Enter() override {}
	void Exit() override {}
	void GetInput() override;
	void UpdateState() override;
	void Render() override;
private:

};

class PlayGameState : public GameState
{
friend class Game;
public:
	PlayGameState() = default;
	~PlayGameState() = default;

	void Enter() override;
	void Exit() override;
	void GetInput() override;
	void UpdateState() override;
	void Render() override;
private:
	static Game *game;
	static void UpdateStateSyncWithAudio();
};

class Game
{
friend class PlayGameState;
public:
	static Game &GetInstance();

	~Game();

	void Initialize(int windowWidth, int windowHeight);
	void Run();
private:
	Game() = default;

	void Loop();
	void GetInput() { stateStack.Top()->GetInput(); }
	void UpdateState() { stateStack.Top()->UpdateState(); }
	void Render() { stateStack.Top()->Render(); }

	NES nes;

	/**** game FSM ****/
	MenuGameState menuGameState;
	PlayGameState playGameState;

	void ChangeState(GameState *state);
	void PushState(GameState *state);
	void PopState();

	Stack<GameState*> stateStack;
	bool mQuit = false;
};

#endif  // GAME_H