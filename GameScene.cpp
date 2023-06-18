#include "GameScene.h"
#include "DirectXCommon.h"
#include "Matrix4x4.h"

#include "ImGuiManager.h"

GameScene::GameScene(){}

GameScene::~GameScene(){}

void GameScene::Initialize(){
	dxCommon_ = DirectXCommon::GetInstance();

	// デバックカメラ
	debugCamera_ = new DebugCamera();

	// 入力
	input_ = Input::GetInstans();

	// テクスチャ
	textureHandle_ = TextureManager::Load("resources/uvChecker.png");
	textureHandle_2_ = TextureManager::Load("resources/mario.png");

	// カメラの初期化
	viewProjection_.Initialize();

	// 土台
	base_ = Basic::Create();
	baseWorldTransform_.Initialize();
	baseWorldTransform_.scale_ = { 4.0f,0.5,4.0f };
	baseWorldTransform_.translation_ = { 0.0f,-2.0f,0.0f };
	baseWorldTransform_.UpdateMatrix();

	// パーティクル
	billParticle_ = new BillParticle();
}

void GameScene::Update(){
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);

	if (input_->PushKey(DIK_SPACE)) {
 		billParticle_->SetEmitter(Vector3(0.0f,0.0f,0.0f));
		billParticle_->Create();
	}

	billParticle_->Update(viewProjection_);
}

void GameScene::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Basic::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	base_->Draw(baseWorldTransform_, viewProjection_);
	
	// 3Dオブジェクト描画後処理
	Basic::PostDraw();
#pragma endregion
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	billParticle_->Draw(viewProjection_);

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Release(){
	SafeDelete(base_);
	SafeDelete(billParticle_); 
	SafeDelete(debugCamera_);
}
