#include "GameScene.h"

#include "DirectXCommon.h"
#include "Draw.h"
#include "GlobalVariables.h"
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
	playerAttack_ = std::make_unique<PlayerAttack>();
	skydome_ = std::make_unique<Skydome>();
#pragma endregion
	// 敵
	// 敵モデル
	std::vector<std::vector<Model*>> enemyModels;
	// 敵攻撃モデル
	std::vector<Model*> enemyAttackModel(static_cast<int>(EnemyAttack::Parts::COUNT));
	// 敵モデル
	for (size_t i = 0; i < 5; i++) {
		std::vector<Model*> enemyModel(static_cast<int>(Enemy::Parts::COUNT));
		enemyModel[static_cast<int>(Enemy::Parts::BODY)] = (Model::Create("enemy_Body", true));
		enemyModel[static_cast<int>(Enemy::Parts::HEAD)] = (Model::Create("enemy_Head", true));
		enemyModel[static_cast<int>(Enemy::Parts::ARML)] = (Model::Create("enemy_armL", true));
		enemyModel[static_cast<int>(Enemy::Parts::ARMR)] = (Model::Create("enemy_armR", true));
		enemyModels.emplace_back(enemyModel);
	}
	// 敵攻撃モデル
	enemyAttackModel[static_cast<int>(EnemyAttack::Parts::CIRCLE)] = (Model::Create("enemy_Attack_Circle", true));
	enemyAttackModel[static_cast<int>(EnemyAttack::Parts::PLANE)] = (Model::Create("enemy_Attack_Plane", true));
	enemyAttackModel[static_cast<int>(EnemyAttack::Parts::METEO)] = (Model::Create("enemy_Meteo", true));
	// 敵初期化
	for (size_t i = 0; i < 5; i++) {
		EnemyAttack* enemyAttack = new EnemyAttack();
		Enemy* enemy = new Enemy();
		EnemyHP* enemyHP = new EnemyHP();
		enemy->Initialize(enemyModels.at(i));
		enemy->SetPlayer(player_.get());
		enemy->SetPlayerAttack(playerAttack_.get());
		enemy->SetEnemyAttack(enemyAttack);
		enemy->SetEnemyHP(enemyHP);
		// 敵攻撃初期化
		enemyAttack->SetPlayerEnemy(player_.get(), enemy);
		enemyAttack->Initialize(enemyAttackModel);
		// 
		enemyHP->Initialize();

		enemy_.emplace_back(enemy);
		enemyAttack_.emplace_back(enemyAttack);
		enemyHP_.emplace_back(enemyHP);
	}
	enemy_.at(0)->SetPosition({ 30.0f,0.0f,5.0f });
	enemy_.at(1)->SetPosition({ 30.0f,0.0f,20.0f });
	enemy_.at(2)->SetPosition({ 30.0f,0.0f,35.0f });
	enemy_.at(3)->SetPosition({ 45.0f,0.0f,35.0f });
	enemy_.at(4)->SetPosition({ 45.0f,0.0f,50.0f });
	// プレイヤー
	std::vector<Model*> playerModel(static_cast<int>(Player::Parts::COUNT));
	std::vector<Model*> playerAttackModel(static_cast<int>(Player::Parts::COUNT));
	// プレイヤーモデル
	playerModel[static_cast<int>(Player::Parts::HEAD)] = (Model::Create("head", true));
	playerModel[static_cast<int>(Player::Parts::BODY)] = (Model::Create("body", true));
	playerModel[static_cast<int>(Player::Parts::ARML)] = (Model::Create("armL", true));
	playerModel[static_cast<int>(Player::Parts::ARMR)] = (Model::Create("armR", true));
	playerModel[static_cast<int>(Player::Parts::WEAPON)] = (Model::Create("player_Weapon", true));
	playerAttackModel[static_cast<int>(PlayerAttack::Parts::WEAPON)] = (Model::Create("player_Weapon", true));
	// プレイヤー初期化
	player_->SetViewProjection(&viewProjection_);
	player_->SetPlayerAttack(playerAttack_.get());
	player_->SetEnemyAttack(enemyAttack_);
	player_->Initialize(playerModel);

	playerAttack_->SetPlayer(player_.get());
	playerAttack_->Initialize(playerAttackModel);

	// ブロック
	std::vector<Model*> blockModel(1);
	// ブロックモデル
	blockModel[0] = (Model::Create("block", true));
	// ブロック初期化
	block_->Initialize(blockModel);
	block_->SetPlayer(player_.get());

	// 天球モデル
	std::unique_ptr<Model> skydomeModel;
	// 天球のモデル
	skydomeModel.reset(Model::Create("sky", true, false));
	// 天球初期化
	skydome_->Initialize(std::move(skydomeModel));

	//フォローカメラ
	followCamera_.Intialize();
	followCamera_.SetTarget(&player_.get()->GetWorldTransform());
	for (int i = 0; i < enemy_.size(); i++) {
		followCamera_.SetEnemy(enemy_.at(i));
	}
}

void GameScene::Update() {
	GlobalVariables::GetInstance()->Update();
	// デバックカメラ
	//debugCamera_->Update(&viewProjection_);
	// フォローカメラ	
	followCamera_.Update();
	viewProjection_ = *followCamera_.GetViewProjection();
	// ブロック
	block_->Update();
	// プレイヤー
	playerAttack_->Update();
	player_->Update();
	// 敵
	for (size_t i = 0; i < 5; i++) {
		enemyAttack_.at(i)->Update();
		enemyHP_.at(i)->Update();
		enemy_.at(i)->Update();
	}

	// コリジョンマネージャー
	collisionManager_->Update(player_.get(), playerAttack_.get(), block_.get(), enemy_, enemyAttack_);
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
	//skydome_->Draw(viewProjection_);
	block_->Draw(viewProjection_);
	for (size_t i = 0; i < 5; i++) {
		enemy_.at(i)->Draw(viewProjection_);
		enemyAttack_.at(i)->Draw(viewProjection_);
	}
	player_->Draw(viewProjection_);
	if (player_->GetBehavior() == Player::Behavior::kAttack) {
		playerAttack_->Draw(viewProjection_);
	}
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
	for (size_t i = 0; i < 5; i++) {
		SafeDelete(enemy_.at(i));
		SafeDelete(enemyAttack_.at(i));
	}
}
