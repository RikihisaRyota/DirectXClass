#include "GameScene.h"

#include "DirectXCommon.h"
#include "Draw.h"
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
	enemy_ = std::make_unique<Enemy>();
	enemyAttack_ = std::make_unique<EnemyAttack>();
	player_ = std::make_unique<Player>();
	playerAttack_ = std::make_unique<PlayerAttack>();
	skydome_ = std::make_unique<Skydome>();
#pragma endregion
	// プレイヤー
	std::vector<std::unique_ptr<Model>> playerModel(static_cast<int>(Player::Parts::COUNT));
	std::vector<std::unique_ptr<Model>> playerAttackModel(static_cast<int>(Player::Parts::COUNT));
	// プレイヤーモデル
	playerModel[static_cast<int>(Player::Parts::HEAD)].reset(Model::Create("head", true));
	playerModel[static_cast<int>(Player::Parts::BODY)].reset(Model::Create("body", true));
	playerModel[static_cast<int>(Player::Parts::ARML)].reset(Model::Create("armL", true));
	playerModel[static_cast<int>(Player::Parts::ARMR)].reset(Model::Create("armR", true));
	playerModel[static_cast<int>(Player::Parts::WEAPON)].reset(Model::Create("player_Weapon", true));
	playerAttackModel[static_cast<int>(PlayerAttack::Parts::WEAPON)].reset(Model::Create("player_Weapon", true));
	// プレイヤー初期化
	player_->SetViewProjection(&viewProjection_);
	player_->SetPlayerAttack(playerAttack_.get());
	player_->SetEnemy(enemy_.get());
	player_->SetEnemyAttack(enemyAttack_.get());
	player_->Initialize(std::move(playerModel));

	playerAttack_->Initialize(std::move(playerAttackModel));

	// ブロック
	std::vector<std::unique_ptr<Model>> blockModel(1);
	// ブロックモデル
	blockModel[0].reset(Model::Create("block", true));
	// ブロック初期化
	block_->Initialize(std::move(blockModel));
	block_->SetPlayer(player_.get());
	// 敵
	// 敵モデル
	std::vector<std::unique_ptr<Model>> enemyModel(static_cast<int>(Enemy::Parts::COUNT));
	// 敵攻撃モデル
	std::vector<std::unique_ptr<Model>> enemyAttackModel(
		static_cast<int>(EnemyAttack::Parts::COUNT));
	// 敵モデル
	enemyModel[static_cast<int>(Enemy::Parts::BODY)].reset(Model::Create("enemy_Body", true));
	enemyModel[static_cast<int>(Enemy::Parts::HEAD)].reset(Model::Create("enemy_Head", true));
	enemyModel[static_cast<int>(Enemy::Parts::ARML)].reset(Model::Create("enemy_armL", true));
	enemyModel[static_cast<int>(Enemy::Parts::ARMR)].reset(Model::Create("enemy_armR", true));

	// 敵攻撃モデル
	enemyAttackModel[static_cast<int>(EnemyAttack::Parts::CIRCLE)].reset(
		Model::Create("enemy_Attack_Circle", true));
	enemyAttackModel[static_cast<int>(EnemyAttack::Parts::PLANE)].reset(
		Model::Create("enemy_Attack_Plane", true));
	enemyAttackModel[static_cast<int>(EnemyAttack::Parts::METEO)].reset(
		Model::Create("enemy_Meteo", true));
	// 敵初期化
	enemy_->Initialize(std::move(enemyModel));
	enemy_->SetPlayer(player_.get());
	enemy_->SetEnemyAttack(enemyAttack_.get());
	// 敵攻撃初期化
	enemyAttack_->SetPlayerEnemy(player_.get(), enemy_.get());
	enemyAttack_->Initialize(std::move(enemyAttackModel));

	// 天球モデル
	std::unique_ptr<Model> skydomeModel;
	// 天球のモデル
	skydomeModel.reset(Model::Create("sky", true, false));
	// 天球初期化
	skydome_->Initialize(std::move(skydomeModel));

	//フォローカメラ
	followCamera_.Intialize();
	followCamera_.SetTarget(&player_.get()->GetWorldTransform());
}

void GameScene::Update() {
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
	// フォローカメラ	
	followCamera_.Update();
	viewProjection_ = *followCamera_.GetViewProjection();
	// ブロック
	block_->Update();
	// プレイヤー
	player_->Update();
	// 敵
	enemy_->Update();
	enemyAttack_->Update();
	// コリジョンマネージャー
	collisionManager_->Update(player_.get(),block_.get(),enemy_.get(),enemyAttack_.get());
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
	skydome_->Draw(viewProjection_);
	block_->Draw(viewProjection_);
	enemy_->Draw(viewProjection_);
	enemyAttack_->Draw(viewProjection_);
	player_->Draw(viewProjection_);
	// パーティクル
	playerAttack_->ParticleDraw(viewProjection_);

	//block_->HitBoxDraw(viewProjection_);
	//player_->HitBoxDraw(viewProjection_);
	//enemy_->HitBoxDraw(viewProjection_);
	//enemyAttack_->HitBoxDraw(viewProjection_);
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
