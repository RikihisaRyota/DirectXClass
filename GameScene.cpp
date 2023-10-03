#include "GameScene.h"
#include "DirectXCommon.h"
#include "Matrix4x4.h"
#include "TextureManager.h"
#include "ImGuiManager.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();

	// デバックカメラ
	debugCamera_ = new DebugCamera();

	// 入力
	input_ = Input::GetInstans();

	// ライト
	directionalLight_ = new cDirectionalLight();
	directionalLight_->color_ = { 1.0f,1.0f,1.0f };
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;

	// カメラの初期化
	viewProjection_.Initialize();
	Line::GetInstance()->SetViewProjection(&viewProjection_);

	block_ = std::make_unique<Block>(Model::Create("block"));

	// 音声再生
	audio = new Audio;
	audio->Initialize();
	soundHandle_ = audio->SoundLoadWave("resources/mokugyo.wav");
}

void GameScene::Update() {
	// 音声テスト
	//audio->SoundPlayWave(soundHandle_);
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
}

void GameScene::Draw() {

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
	dxCommon_->ClearDepthBuffer();
#pragma endregion
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Cube::PreDraw(commandList);
	Sphere::PreDraw(commandList);
	OBJ::PreDraw(commandList);
	Model::PreDraw(commandList);
	Line::PreDraw(commandList);
	Plane::PreDraw(commandList);

	


	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	Line::GetInstance()->Line::Draw();
	// 3Dオブジェクト描画後処理
	Plane::PostDraw();
	Line::PostDraw();
	Model::PostDraw();
	Sphere::PostDraw();
	OBJ::PostDraw();
	Cube::PostDraw();
#pragma endregion



#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Release() {
	SafeDelete(debugCamera_);
	SafeDelete(directionalLight_);
}
