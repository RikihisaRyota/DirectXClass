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
#pragma region カメラ
	/*ImGui::Begin("camera");
	ImGui::SliderFloat3(
		" translation_",
		&viewProjection_.translation_.x,
		-50.0f,
		0.0f);
	ImGui::SliderFloat3(
		" rotation_",
		&viewProjection_.rotation_.x,
		-2.0f,
		2.0f);
	ImGui::End();
	viewProjection_.UpdateMatrix();*/
#pragma endregion カメラ
	ImGui::Begin("camera");
	ImGui::Text(
		" matView_ \n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",
		viewProjection_.matView_.m[0][0], viewProjection_.matView_.m[0][1],viewProjection_.matView_.m[0][2], viewProjection_.matView_.m[0][3],
		viewProjection_.matView_.m[1][0], viewProjection_.matView_.m[1][1],viewProjection_.matView_.m[1][2], viewProjection_.matView_.m[1][3],
		viewProjection_.matView_.m[2][0], viewProjection_.matView_.m[2][1],viewProjection_.matView_.m[2][2], viewProjection_.matView_.m[2][3],
		viewProjection_.matView_.m[3][0], viewProjection_.matView_.m[3][1],viewProjection_.matView_.m[3][2], viewProjection_.matView_.m[3][3]
	);
	ImGui::End();
	ImGui::Begin("worldTransform_1_");
	ImGui::Text(
		" rotation_ : %f,%f,%f",
		worldTransform_1_.rotation_.x,
		worldTransform_1_.rotation_.y,
		worldTransform_1_.rotation_.z
		);
	ImGui::Text(
		" matView_ \n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",
		worldTransform_1_.matWorld_.m[0][0], worldTransform_1_.matWorld_.m[0][1], worldTransform_1_.matWorld_.m[0][2], worldTransform_1_.matWorld_.m[0][3],
		worldTransform_1_.matWorld_.m[1][0], worldTransform_1_.matWorld_.m[1][1], worldTransform_1_.matWorld_.m[1][2], worldTransform_1_.matWorld_.m[1][3],
		worldTransform_1_.matWorld_.m[2][0], worldTransform_1_.matWorld_.m[2][1], worldTransform_1_.matWorld_.m[2][2], worldTransform_1_.matWorld_.m[2][3],
		worldTransform_1_.matWorld_.m[3][0], worldTransform_1_.matWorld_.m[3][1], worldTransform_1_.matWorld_.m[3][2], worldTransform_1_.matWorld_.m[3][3]
	);
	ImGui::End();
	
	// ビューマトリックスの逆行列作成
	Matrix4x4 cameraInverse = Inverse(viewProjection_.matView_);
	// 移動情報の打ち消し
	cameraInverse.m[3][0] = 0.0f;
	cameraInverse.m[3][1] = 0.0f;
	cameraInverse.m[3][2] = 0.0f;
	Matrix4x4 worldTransformAffin = MakeAffineMatrix(worldTransform_1_.scale_, worldTransform_1_.rotation_, worldTransform_1_.translation_);
	worldTransform_1_.matWorld_ = worldTransformAffin * cameraInverse;
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
