#pragma once
#include <memory>
#include <optional>

#include "TitleScene.h"
#include "GameClear.h"
#include "GameOver.h"
#include "GameScene.h"

class SceneManager {
public:
	enum class State {
		TITLE,
		INGAME,
		GAMEOVER,
		GAMECLEAR,
	};
public:
	SceneManager(uint32_t textureHandle);
	~SceneManager();
	static void SetState(State state) { stateRequest_ = state; }
	static State GetState() {return state_; }

	void Initialize();
	void Update();
	void Draw();
private:
	static std::optional<State> stateRequest_;
	std::unique_ptr<TitleScene> titleScene_;
	std::unique_ptr<GameScene> gameScene_;
	std::unique_ptr<GameOver> gameOver_;
	std::unique_ptr<GameClear> gameClear_;
	static State state_;
	State preState_ = State::TITLE;
	std::unique_ptr<Sprite> back_Sprite = nullptr;

	bool IsStart_;
	bool IsDark_;
	bool IsInitialize_;
	float t_;
	float t_Speed_;
};