#include "GameScene.h"
#include "DirectXCommon.h"
#include "Matrix4x4.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();

	// ロード
	textureHandle_ = TextureManager::Load("resources/uvChecker.png");

	// デバックカメラ
	debugCamera_ = new DebugCamera();
	// デバックカメラフラグ切り替え
	debugCameraFlag_ = false;
	// 入力
	input_ = Input::GetInstance();
	// 音
	audio_ = Audio::GetInstance();

	ingame_SoundHandle_ = audio_->SoundLoadWave("resources/ingame.wav");

	// ライト
	directionalLight_ = new cDirectionalLight();
	directionalLight_->color_ = { 1.0f,1.0f,1.0f };
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;

	// カメラの初期化
	viewProjection_.Initialize();
#pragma region 天球
	// 天球モデル
	std::unique_ptr<Model> skydomeModel;
	// 天球のモデル
	skydomeModel.reset(Model::Create("sky", true,false));
	// 天球生成
	skydome_ = std::make_unique<Skydome>();
	// 天球初期化
	skydome_->Initialize(std::move(skydomeModel));
#pragma endregion
#pragma region 地面
	// 地面モデル
	std::unique_ptr<Model> groundModel;
	// 地面のモデル
	groundModel.reset(Model::Create("Ground", true, false));
	// 地面生成
	ground_ = std::make_unique<Ground>();
	// 地面初期化
	ground_->Initialize(std::move(groundModel));
#pragma endregion
	// 生成
	player_ = std::make_unique<Player>();
	playerAttack_ = std::make_unique<PlayerAttack>();
	playerHP_ = std::make_unique<PlayerHP>();
	enemy_ = std::make_unique<Enemy>();
	enemyAttack_ = std::make_unique<EnemyAttack>();
	enemyHP_ = std::make_unique<EnemyHP>();
#pragma region 敵
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
	enemy_->SetEnemyHP(enemyHP_.get());
	// 敵攻撃初期化
	enemyAttack_->SetPlayerEnemy(player_.get(), enemy_.get());
	enemyAttack_->Initialize(std::move(enemyAttackModel));
	// 敵HP初期化
	
	enemyHP_->Initialize(static_cast<uint32_t>(TextureManager::TextureHandle::WHITE1x1), static_cast<uint32_t>(TextureManager::TextureHandle::WHITE1x1));
#pragma endregion
#pragma region プレイヤー
	// プレイヤーモデル
	std::vector<std::unique_ptr<Model>> playerModel(static_cast<int>(Player::Parts::COUNT));
	std::vector<std::unique_ptr<Model>> playerAttackModel(
		static_cast<int>(PlayerAttack::Parts::COUNT));
	// プレイヤーモデル
	playerModel[static_cast<int>(Player::Parts::HEAD)].reset(Model::Create("head", true));
	playerModel[static_cast<int>(Player::Parts::BODY)].reset(Model::Create("body", true));
	playerModel[static_cast<int>(Player::Parts::ARML)].reset(Model::Create("armL", true));
	playerModel[static_cast<int>(Player::Parts::ARMR)].reset(Model::Create("armR", true));
	playerModel[static_cast<int>(Player::Parts::WEAPON)].reset(
		Model::Create("player_Weapon", true));
	playerAttackModel[static_cast<int>(PlayerAttack::Parts::WEAPON)].reset(
		Model::Create("player_Weapon", true));
	// プレイヤー初期化
	player_->SetPlayerAttack(playerAttack_.get());
	player_->SetGround(ground_.get());
	player_->SetEnemyAttack(enemyAttack_.get());
	uint32_t chage_Handle = TextureManager::Load("resources/chageAttack.png");
	uint32_t triple_Handle = TextureManager::Load("resources/tripleAttack.png");
	uint32_t dash_Handle = TextureManager::Load("resources/dash.png");
	uint32_t y_buttan_Handle = TextureManager::Load("resources/y_buttan.png");
	uint32_t b_buttan_Handle = TextureManager::Load("resources/b_buttan.png");
	uint32_t x_buttan_Handle = TextureManager::Load("resources/x_buttan.png");
	player_->SetSprite(
		chage_Handle, triple_Handle, dash_Handle,
		static_cast<uint32_t>(TextureManager::TextureHandle::WHITE1x1),
		y_buttan_Handle, b_buttan_Handle, x_buttan_Handle);
	playerAttack_->SetPlayer(player_.get());
	playerAttack_->SetEnemy(enemy_.get());
	player_->Initialize(std::move(playerModel));
	playerAttack_->Initialize(std::move(playerAttackModel));
	playerHP_->Initialize(static_cast<uint32_t>(TextureManager::TextureHandle::WHITE1x1), static_cast<uint32_t>(TextureManager::TextureHandle::WHITE1x1));
#pragma endregion
#pragma region 追従カメラ
	// 追従カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	// 追従カメラ初期化
	followCamera_->Intialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());
	// 敵キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetEnemy(&enemy_->GetWorldTransform());
	// プレイヤーにビュープロジェクションをセット
	player_->SetViewProjection(followCamera_->GetViewProjection());
	//// 一回だけアップデート
	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.SetViewProjection(followCamera_->GetViewProjection());
	viewProjection_.TransferMatrix();
#pragma endregion
}

void GameScene::Update() {
	audio_->SoundPlayLoopStart(ingame_SoundHandle_);
	if (!playerHP_->GetAlive()) {
		audio_->SoundPlayLoopEnd(ingame_SoundHandle_);
		SceneManager::SetState(SceneManager::State::GAMEOVER);
	}
	if (!enemyHP_->GetAlive()) {
		audio_->SoundPlayLoopEnd(ingame_SoundHandle_);
		SceneManager::SetState(SceneManager::State::GAMECLEAR);
	}
	// 敵の更新
	enemyAttack_->Update();
	enemyHP_->Update();
	enemy_->Update();
	// プレイヤーの更新
	playerAttack_->Update();
	playerHP_->Update();
	player_->Update();

	collisionManager.Update(player_.get(), playerAttack_.get(), enemy_.get(), enemyAttack_.get());

	skydome_->Update();
	ground_->Update();
#pragma region カメラ関連
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		debugCameraFlag_ ^= true;
	}
	if (debugCameraFlag_) {
		// デバックカメラの更新
		debugCamera_->Update(&viewProjection_);
		viewProjection_.SetViewProjection(&viewProjection_);
		viewProjection_.TransferMatrix();

	}
	else {
		// 追従カメラの更新
		followCamera_->Update();
		viewProjection_.SetViewProjection(followCamera_->GetViewProjection());
		viewProjection_.TransferMatrix();
	}
#pragma endregion
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
	ModelSphere::PreDraw(commandList);
	OBJ::PreDraw(commandList);
	Model::PreDraw(commandList);
	PrimitiveDrawer::PreDraw(commandList);
	Plate::PreDraw(commandList);

	


	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 天球描画
	skydome_->Draw(viewProjection_);
	// 地面描画
	ground_->Draw(viewProjection_);
	// プレイヤー描画
	player_->Draw(viewProjection_);
	if (player_->GetBehavior() == Player::Behavior::kAttack) {
		playerAttack_->Draw(viewProjection_);
	}

	// 敵描画
	enemy_->Draw(viewProjection_);
	if (enemy_->GetBehavior() == Enemy::Behavior::kAttack) {
		enemyAttack_->Draw(viewProjection_);
	}
	// パーティクル
	playerAttack_->ParticleDraw(viewProjection_);
	//////////////Debug//////////////////
	/*player_->HitBoxDraw(viewProjection_);
	if (player_->GetBehavior() == Player::Behavior::kAttack) {
		playerAttack_->HitBoxDraw(viewProjection_);
	}
	enemy_->HitBoxDraw(viewProjection_);
	if (enemy_->GetBehavior() == Enemy::Behavior::kAttack) {
		enemyAttack_->HitBoxDraw(viewProjection_);
	}*/


	PrimitiveDrawer::GetInstance()->PrimitiveDrawer::Draw();
	// 3Dオブジェクト描画後処理
	Plate::PostDraw();
	PrimitiveDrawer::PostDraw();
	Model::PostDraw();
	ModelSphere::PostDraw();
	OBJ::PostDraw();
	Cube::PostDraw();
#pragma endregion



#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	player_->DrawUI();
	playerHP_->Draw();
	enemyHP_->Draw();
	// test->Draw();
	//  スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Release() {
	SafeDelete(debugCamera_);
	SafeDelete(directionalLight_);
	playerAttack_->ParticleRelease();
}
