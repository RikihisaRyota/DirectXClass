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
	block_ = std::make_unique<Block>();
	collisionManager_ = std::make_unique<CollisionManager>();
	player_ = std::make_unique<Player>();
#pragma endregion
	// プレイヤー
	std::vector<std::unique_ptr<Model>> playerModel(static_cast<int>(Player::Parts::COUNT));
	// プレイヤーモデル
	playerModel[static_cast<int>(Player::Parts::HEAD)].reset(Model::Create("head", true));
	playerModel[static_cast<int>(Player::Parts::BODY)].reset(Model::Create("body", true));
	playerModel[static_cast<int>(Player::Parts::ARML)].reset(Model::Create("armL", true));
	playerModel[static_cast<int>(Player::Parts::ARMR)].reset(Model::Create("armR", true));
	playerModel[static_cast<int>(Player::Parts::WEAPON)].reset(Model::Create("player_Weapon", true));
	// プレイヤー初期化
	player_->SetViewProjection(&viewProjection_);
	player_->Initialize(std::move(playerModel));

	// ブロック
	std::vector<std::unique_ptr<Model>> blockModel(1);
	// ブロックモデル
	blockModel[0].reset(Model::Create("block", true));
	// ブロック初期化
	block_->Initialize(std::move(blockModel));

	//フォローカメラ
	followCamera_.Intialize();
	followCamera_.SetTarget(&player_.get()->GetWorldTransform());
}

void GameScene::Update() {
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
	// フォローカメラ	
	/*followCamera_.Update();
	viewProjection_ = *followCamera_.GetViewProjection();*/
	// プレイヤー
	player_->Update();
	// ブロック
	block_->Update();
	// コリジョンマネージャー
	collisionManager_->Update(player_.get(),block_.get());
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
	PrimitiveDrawer::PreDraw(commandList);
	PlaneRenderer::PreDraw(commandList);
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	player_->Draw(viewProjection_);
	block_->Draw(viewProjection_);

	block_->HitBoxDraw(viewProjection_);
	player_->HitBoxDraw(viewProjection_);
	PrimitiveDrawer::GetInstance()->PrimitiveDrawer::Draw();
	// 3Dオブジェクト描画後処理
	PlaneRenderer::PostDraw();
	PrimitiveDrawer::PostDraw();
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
