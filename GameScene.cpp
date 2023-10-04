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
	input_ = Input::GetInstance();
	// カメラの初期化
	viewProjection_.Initialize();
#pragma region 生成
	player_ = std::make_unique<Player>();
	cube_ = std::make_unique<Cube>();
	sprite_ = std::make_unique<Sprite>();
#pragma endregion
	cube_.reset(Cube::Create());
	cubeWorldTransform_.Initialize();
	sprite_.reset(Sprite::Create(0, { 0.0f,0.0f }));
	sprite_->SetSize({ 500.0f,500.0f });
	// プレイヤー
	std::vector<std::unique_ptr<Model>> playerModel(static_cast<int>(Player::Parts::COUNT));
	// プレイヤーモデル
	playerModel[static_cast<int>(Player::Parts::HEAD)].reset(Model::Create("head", true));
	playerModel[static_cast<int>(Player::Parts::BODY)].reset(Model::Create("body", true));
	playerModel[static_cast<int>(Player::Parts::ARML)].reset(Model::Create("armL", true));
	playerModel[static_cast<int>(Player::Parts::ARMR)].reset(Model::Create("armR", true));
	playerModel[static_cast<int>(Player::Parts::WEAPON)].reset(Model::Create("player_Weapon", true));
	// プレイヤー初期化
	//player_->SetGround(ground_.get());
	player_->SetViewProjection(&viewProjection_);
	player_->Initialize(std::move(playerModel));
}

void GameScene::Update() {
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
	// プレイヤー
	player_->Update();
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
	SphereRenderer::PreDraw(commandList);
	OBJ::PreDraw(commandList);
	Model::PreDraw(commandList);
	LineRenderer::PreDraw(commandList);
	PlaneRenderer::PreDraw(commandList);
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	player_->Draw(viewProjection_);
	cube_->Draw(cubeWorldTransform_,viewProjection_);
	LineRenderer::GetInstance()->LineRenderer::Draw();
	// 3Dオブジェクト描画後処理
	PlaneRenderer::PostDraw();
	LineRenderer::PostDraw();
	Model::PostDraw();
	SphereRenderer::PostDraw();
	OBJ::PostDraw();
	Cube::PostDraw();
#pragma endregion
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	Sprite::SetBlendState(Sprite::BlendState::kNormal);
	//sprite_->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Release() {
	SafeDelete(debugCamera_);
}
