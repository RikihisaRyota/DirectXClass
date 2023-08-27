#include "SceneManager.h"

#include "DirectXCommon.h"
#include "MyMath.h"

std::optional<SceneManager::State> SceneManager::stateRequest_ = std::nullopt;

SceneManager::SceneManager(uint32_t textureHandle) { 
	titleScene_ = std::make_unique<TitleScene>();
	gameScene_ = std::make_unique<GameScene>();
	gameOver_ = std::make_unique<GameOver>();
	gameClear_ = std::make_unique<GameClear>();
	titleScene_->Initialize(); 
	gameScene_->Initialize(); 
	gameOver_->Initialize(); 
	gameClear_->Initialize(); 
	back_Sprite.reset(Sprite::Create(textureHandle,Vector2(0.0f,0.0f),Vector4(1.0f,1.0f,1.0f,0.0f)));
	back_Sprite->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
	IsStart_ = true;
	IsDark_ = false;
	t_ = 1.0f;
	t_Speed_ = 0.05f;
}

SceneManager::~SceneManager() {}

void SceneManager::Initialize() { 
	switch (state_) {
	case SceneManager::State::TITLE:
		titleScene_->Initialize();
		break;
	case SceneManager::State::INGAME:
		gameScene_->Initialize();
		break;
	case SceneManager::State::GAMEOVER:
		gameOver_->Initialize();
		break;
	case SceneManager::State::GAMECLEAR:
		gameClear_->Initialize();
		break;
	default:
		break;
	}
	// ふるまいリクエストをリセット
	stateRequest_ = std::nullopt;
	t_ = 0.0f;
	IsStart_ = false;
	IsDark_ = false;
}

void SceneManager::Update() {
	if (stateRequest_) {
		// 前回のふるまいを設定
		preState_ = state_; 
		// ふるまいを変更
		state_ = stateRequest_.value();
		Initialize();
	}
	if (IsStart_) {
		switch (state_) {
		case SceneManager::State::TITLE:
			titleScene_->Update();
			break;
		case SceneManager::State::INGAME:
			gameScene_->Update();
			break;
		case SceneManager::State::GAMEOVER:
			gameOver_->Update();
			break;
		case SceneManager::State::GAMECLEAR:
			gameClear_->Update();
			break;
		default:
			break;
		}
	} else {
		t_ += t_Speed_;
		if (!IsDark_) {
			Vector4 color{0.0f, 0.0f, 0.0f, Lerp(0.0f, 1.0f, Clamp(t_, 0.0f, 1.0f))};
			back_Sprite->SetColor(color);
			if (t_ >= 1.0f) {
				IsDark_ = true;
				t_ = 0.0f;
			}
		}
		if (IsDark_) {
			Vector4 color{0.0f, 0.0f, 0.0f, Lerp(1.0f, 0.0f, Clamp(t_, 0.0f, 1.0f))};
			back_Sprite->SetColor(color);
			if (t_ >= 1.0f) {
				IsStart_ = true;
			}
		}
	}
}

void SceneManager::Draw() {
	State nowState{};
	if (!IsDark_) {
		nowState = preState_;
	} else {
		nowState = state_;
	}
	switch (nowState) {
	case SceneManager::State::TITLE:
		titleScene_->Draw();
		break;
	case SceneManager::State::INGAME:
		gameScene_->Draw();
		break;
	case SceneManager::State::GAMEOVER:
		gameOver_->Draw();
		break;
	case SceneManager::State::GAMECLEAR:
		gameClear_->Draw();
		break;
	default:
		break;
	}
	
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();
#pragma endregion
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	/// // 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	back_Sprite->Draw();

	//  スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
