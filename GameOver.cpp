#include "GameOver.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "SceneManager.h"

#include "ImGuiManager.h"

GameOver::GameOver() {}

GameOver::~GameOver() {}

void GameOver::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	//audio_ = Audio::GetInstance();
	viewProjection_.Initialize();

	over_Sprite_TextureHandle_ = TextureManager::Load("resources./over.png");
	over_Sprite_.reset(Sprite::Create(
	    over_Sprite_TextureHandle_,
	    Vector2(WinApp::kWindowWidth * 0.5f, WinApp::kWindowHeight * 0.25f),
	    Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2(0.5f,0.5f))
	);
	over_scale_ = { static_cast<float>(WinApp::kWindowWidth) * 0.4f,static_cast<float>(WinApp::kWindowHeight) * 0.4f };
	over_Sprite_->SetSize(over_scale_);
	pless_b_Sprite_TextureHandle_= TextureManager::Load("resources./pless_b.png");
	pless_b_Sprite_.reset(Sprite::Create(
		pless_b_Sprite_TextureHandle_,
		Vector2(WinApp::kWindowWidth * 0.5f, WinApp::kWindowHeight * 0.75f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2(0.5f, 0.5f))
	);
	black_back_Sprite_.reset(Sprite::Create(
		static_cast<int>(TextureManager::TextureHandle::WHITE1x1),
		Vector2(0.0f,0.0f),
		Vector4(0.0f, 0.0f, 0.0f, 1.0f)));
	black_back_Sprite_->SetSize(Vector2(static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight)));
	
	pless_b_Flag_ = false;
	color_flag_ = false;
	count_max_ = 4;
	count_ = count_max_;
}

void GameOver::Update() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	//
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) ||
	    (Input::GetInstance()->GetJoystickState(0, joyState) &&
	     (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
		SceneManager::SetState(SceneManager::State::TITLE);
		pless_b_Flag_ = true;
	}
	
	ImGui::Begin("gameover");
	ImGui::SliderFloat2("over_scale_", &over_scale_.x, 0.0f, 1280.0f);
	ImGui::SliderFloat2("pless_b_scale_", &pless_b_scale_.x, 0.0f, 1280.0f);
	ImGui::End();
	over_Sprite_->SetSize(over_scale_);
}

void GameOver::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
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
	black_back_Sprite_->Draw();
	over_Sprite_->Draw();
	pless_b_Sprite_->Draw();

	// test->Draw();
	//  スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion 
}

void GameOver::Flash(float count) {
	if (count <= 0.5f) {
		if (pless_b_Flag_) {
			count_--;
			if (count_ <= 0) {
				color_flag_ ^= true;
				count_ = count_max_;
			}
		}
		if (color_flag_) {
			pless_b_Sprite_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else {
			pless_b_Sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
	else {
		pless_b_Flag_ = false;
	}
}