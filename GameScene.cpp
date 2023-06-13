#include "GameScene.h"
#include "DirectXCommon.h"

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

	worldTransform_1_.Initialize();

	worldTransform_2_.Initialize();
	worldTransform_2_.translation_ = { 0.0f,-3.0f,0.0f };
	worldTransform_2_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransform_2_.UpdateMatrix();

	sptiteWorldTransform_.Initialize();
	sptiteWorldTransform_.translation_ = { 0.0f,2.5f,0.0f };
	sptiteWorldTransform_.UpdateMatrix();

	// 生成
	sprite_ = Sprite::Create();

	// 生成
	triangle_1_ = Basic::Create();
	triangle_2_ = Basic::Create();

	// パーティクル
	/*billParticle_ = new BillParticle();*/
}

void GameScene::Update(){
	// ビルボード回転行列
	Matrix4x4 Bill = MakeLookAtLH(worldTransform_1_.translation_,viewProjection_.translation_,Vector3(0.0f,1.0f,0.0f));
	Matrix4x4 InversBill = Inverse(Bill);
	InversBill = NotTransform(InversBill);
	Matrix4x4 worldTransformAffin = MakeAffineMatrix(worldTransform_1_.scale_, worldTransform_1_.rotation_, worldTransform_1_.translation_);
	worldTransform_1_.matWorld_ = worldTransformAffin * InversBill;
	worldTransform_1_.TransferMatrix();

	//
	/*if (input_->PushKey(DIK_SPACE)) {
 		billParticle_->SetEmitter({ 0.0f, 0.0f, 0.0f });
		billParticle_->Create();
	}
	billParticle_->Update();*/
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
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
	triangle_1_->Draw(worldTransform_1_,viewProjection_, textureHandle_);
	triangle_2_->Draw(worldTransform_2_,viewProjection_);

	
	// 3Dオブジェクト描画後処理
	Basic::PostDraw();
#pragma endregion
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	//sprite_->Draw(sptiteWorldTransform_,viewProjection_, textureHandle_2_);
	//billParticle_->Draw(viewProjection_);

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Release(){
	SafeDelete(triangle_1_);
	SafeDelete(triangle_2_);
	SafeDelete(sprite_);
	/*SafeDelete(billParticle_);*/
	SafeDelete(debugCamera_);
}
