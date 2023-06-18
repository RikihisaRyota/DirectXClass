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

	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.UpdateMatrix();

	worldTransform_2_.Initialize();
	worldTransform_2_.translation_ = { 0.0f,-1.0f,0.0f };
	worldTransform_2_.UpdateMatrix();

	// 生成
	triangle_1_ = Basic::Create();
	triangle_2_ = Basic::Create();
}

void GameScene::Update(){
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);

	// ビューマトリックスの逆行列作成
	Matrix4x4 cameraInverse = Inverse(viewProjection_.matView_);
	// 移動情報の打ち消し
	cameraInverse.m[3][0] = 0.0f;
	cameraInverse.m[3][1] = 0.0f;
	cameraInverse.m[3][2] = 0.0f;
	// ワールド座標系
	Matrix4x4 worldTransformAffin = MakeAffineMatrix(
		worldTransform_.scale_, 
		worldTransform_.rotation_, 
		worldTransform_.translation_);
	// ワールド行列
	worldTransform_.matWorld_ = worldTransformAffin * cameraInverse;
	// 行列を転送
	worldTransform_.TransferMatrix();

#pragma region カメラ
	ImGui::Begin("worldTransform_.translation_ ");
	ImGui::Text(
		"translation_\n%f,%f,%f",
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]);
	ImGui::End();
	ImGui::Begin("camera.translation_ ");
	ImGui::Text(
		"translation_\n%f,%f,%f",
		viewProjection_.matView_.m[3][0],
		viewProjection_.matView_.m[3][1],
		viewProjection_.matView_.m[3][2]);
	ImGui::End();
#pragma endregion カメラ
	
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
	triangle_1_->Draw(worldTransform_,viewProjection_);
	triangle_2_->Draw(worldTransform_2_,viewProjection_, textureHandle_);

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
