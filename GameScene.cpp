#include "GameScene.h"

#include "DirectXCommon.h"
#include "MyMath.h"
#include "TextureManager.h"
#include "ImGuiManager.h"

#include "Draw.h"
#include "Sphere.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	// デバックカメラ
	debugCamera_ = new DebugCamera();

	// 入力
	input_ = Input::GetInstans();

	// カメラの初期化
	viewProjection_.Initialize();
	// 線
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
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
	CubeRenderer::PreDraw(commandList);
	SphereRenderer::PreDraw(commandList);
	OBJ::PreDraw(commandList);
	Model::PreDraw(commandList);
	PrimitiveDrawer::PreDraw(commandList);
	PlaneRenderer::PreDraw(commandList);




	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	DrawLine(Vector3(0.0f,0.0f,0.0f), Vector3(1.0f, 0.0f, 0.0f),viewProjection_,Vector4(1.0f,0.0f,0.0f,1.0f));
	DrawLine(Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f),viewProjection_,Vector4(0.0f,1.0f,0.0f,1.0f));
	DrawLine(Vector3(1.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f),viewProjection_,Vector4(0.0f,0.0f,1.0f,1.0f));
	DrawLine(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f),viewProjection_,Vector4(0.0f,0.0f,0.0f,1.0f));
	
	DrawSphere(Sphere({0.0f,0.0f,0.0f},1.0f), viewProjection_, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	
	PrimitiveDrawer::Draw();
	// 3Dオブジェクト描画後処理
	PlaneRenderer::PostDraw();
	PrimitiveDrawer::PostDraw();
	Model::PostDraw();
	SphereRenderer::PostDraw();
	OBJ::PostDraw();
	CubeRenderer::PostDraw();
#pragma endregion



#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	Sprite::SetBlendState(Sprite::BlendState::kNormal);
	
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion

}

void GameScene::Release() {
	SafeDelete(debugCamera_);
}
