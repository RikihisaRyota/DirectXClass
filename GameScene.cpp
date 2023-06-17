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
	worldTransform_1_.translation_ = { 0.0f,0.0f,0.0f };
	worldTransform_1_.UpdateMatrix();

	worldTransform_2_.Initialize();
	worldTransform_2_.translation_ = { 0.0f,-3.0f,0.0f };
	worldTransform_2_.UpdateMatrix();

	// 生成
	triangle_1_ = Basic::Create();
	triangle_2_ = Basic::Create();
}

void GameScene::Update(){
	// ビューマトリックスの逆行列作成
	Matrix4x4 cameraInverse = Inverse(viewProjection_.matView_);
	// 移動情報の打ち消し
	cameraInverse.m[3][0] = 0.0f;
	cameraInverse.m[3][1] = 0.0f;
	cameraInverse.m[3][2] = 0.0f;
	Matrix4x4 worldTransformAffin = MakeAffineMatrix(worldTransform_1_.scale_, worldTransform_1_.rotation_, worldTransform_1_.translation_);
	worldTransform_1_.matWorld_ = cameraInverse * worldTransformAffin;
	worldTransform_1_.TransferMatrix();

	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
}

void GameScene::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Basic::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	triangle_1_->Draw(worldTransform_1_,viewProjection_, textureHandle_);
	triangle_2_->Draw(worldTransform_2_,viewProjection_, textureHandle_2_);

	//
	// スプライト描画後処理
	Basic::PostDraw();

#pragma endregion
}

void GameScene::Release(){
	SafeDelete(triangle_1_);
	SafeDelete(triangle_2_);
	SafeDelete(debugCamera_);
}
