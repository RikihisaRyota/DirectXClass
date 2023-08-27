#include "TitleScene.h"

#include "SceneManager.h"
#include "TextureManager.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	viewProjection_.Initialize();
	titie_Sprite_TextureHandle_ = TextureManager::Load("title.png");
	titie_Sprite_.reset(Sprite::Create(
	    titie_Sprite_TextureHandle_, Vector2(WinApp::kWindowWidth*0.5f, WinApp::kWindowHeight*0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
}

void TitleScene::Update() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	// 
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) ||
	    (Input::GetInstance()->GetJoystickState(0, joyState) &&
	     (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
		SceneManager::SetState(SceneManager::State::INGAME);
	}
}

void TitleScene::Draw() {
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
	titie_Sprite_->Draw();
	
	//  スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
