#include "TitleScene.h"

#include "DirectXCommon.h"
#include "SceneManager.h"
#include "TextureManager.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_->Initialize();

	viewProjection_.Initialize();
	titie_Sprite_TextureHandle_ = TextureManager::Load("resources/title.png");
	titie_Sprite_.reset(Sprite::Create(
		titie_Sprite_TextureHandle_, Vector2(WinApp::kWindowWidth * 0.5f, WinApp::kWindowHeight * 0.25f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2(0.5f, 0.5f))
	);
	titie_scale_ = { static_cast<float>(WinApp::kWindowWidth) * 0.4f,static_cast<float>(WinApp::kWindowHeight) * 0.4f };
	titie_Sprite_->SetSize(titie_scale_);
	pless_b_Sprite_TextureHandle_ = TextureManager::Load("resources./pless_b.png");
	pless_b_Sprite_.reset(Sprite::Create(
		pless_b_Sprite_TextureHandle_,
		Vector2(WinApp::kWindowWidth * 0.5f, WinApp::kWindowHeight * 0.75f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2(0.5f, 0.5f))
	);

	pless_b_Flag_ = false;
	color_flag_ = false;
	count_max_ = 4;
	count_ = count_max_;

	// カメラの初期化
	viewProjection_.Initialize();
#pragma region 天球
	// 天球モデル
	std::unique_ptr<Model> skydomeModel;
	// 天球のモデル
	skydomeModel.reset(Model::Create("sky", true, false));
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
#pragma region プレイヤー
	// 生成
	player_ = std::make_unique<Player>();
	playerAttack_ = std::make_unique<PlayerAttack>();
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
		Model::Create("player_Weapon", true, true));
	playerAttackModel[static_cast<int>(PlayerAttack::Parts::WEAPON)].reset(
		Model::Create("player_Weapon", true, true));
	// プレイヤー初期化
	player_->SetPlayerAttack(playerAttack_.get());
	player_->SetGround(ground_.get());
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
	player_->Initialize(std::move(playerModel));
	playerAttack_->Initialize(std::move(playerAttackModel));

#pragma endregion
#pragma region 追従カメラ
	// 追従カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	// 追従カメラ初期化
	followCamera_->Intialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());
	// プレイヤーにビュープロジェクションをセット
	player_->SetViewProjection(followCamera_->GetViewProjection());
	//// 一回だけアップデート
	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.SetViewProjection(followCamera_->GetViewProjection());
	viewProjection_.TransferMatrix();
#pragma endregion
}

void TitleScene::Update() {
#pragma region カメラ関連
	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.SetViewProjection(followCamera_->GetViewProjection());
	viewProjection_.TransferMatrix();
#pragma endregion
	// プレイヤーの更新
	player_->Update();
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	// 
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		(Input::GetInstance()->GetJoystickState(0, joyState) &&
			(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
		SceneManager::SetState(SceneManager::State::INGAME);
		pless_b_Flag_ = true;
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
	skydome_->Draw(viewProjection_);
	// 地面描画
	ground_->Draw(viewProjection_);
	// プレイヤー描画
	player_->Draw(viewProjection_);

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
	pless_b_Sprite_->Draw();
	//  スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void TitleScene::Flash(float count) {
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
