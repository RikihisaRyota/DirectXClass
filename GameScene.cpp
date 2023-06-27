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
	textureHandle_1_ = TextureManager::Load("resources/gray1x1.png");
	treeTextureHandle_ = TextureManager::Load("resources/tree.png");
	textureHandle_2_ = TextureManager::Load("resources/mario.png");

	// カメラの初期化
	viewProjection_.Initialize();
	viewProjection_.translation_ = Vector3(0.0f, 0.0f, -20.0f); 
	viewProjection_.UpdateMatrix();

	// 土台
	base_ = Basic::Create();
	baseWorldTransform_.Initialize();
	baseWorldTransform_.scale_ = { 8.0f,1.0f,4.0f };
	baseWorldTransform_.translation_ = { 0.0f,-2.0f,0.0f };
	baseWorldTransform_.UpdateMatrix();

	// ビルボード
	billBoard_ = Sprite::Create();
	billBoardTransform_.Initialize();
	billBoardTransform_.scale_ = { 1.0f,1.0f,1.0f };
	billBoardTransform_.translation_ = { 2.0f,0.0f,0.0f };
	billBoardTransform_.UpdateMatrix();
	// ビルボードパーティクル
	billParticle_ = new BillParticle();

	// 木
	tree_ = Sprite::Create();
	treeWorldTransform_.Initialize();
	treeWorldTransform_.scale_ = { 1.0f,2.0f,1.0f };
	treeWorldTransform_.translation_ = { 6.0f,1.0f,0.0f };
	treeWorldTransform_.UpdateMatrix();
	
	// スプライトサンプル
	spriteSample_ = Sprite::Create();
	spriteSampleWorldTransform_.Initialize();
	spriteSampleWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	spriteSampleWorldTransform_.translation_ = { -6.0f,0.0f,0.0f };
	spriteSampleWorldTransform_.UpdateMatrix();
	// スプライトサンプルパーティクル
	spriteSampleParticle_ = new BillParticle();

	// 立方体サンプル
	basicSample_ = Basic::Create();
	basicSampleWorldTransform_.Initialize();
	basicSampleWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	basicSampleWorldTransform_.translation_ = { -2.0f,0.0f,0.0f };
	basicSampleWorldTransform_.UpdateMatrix();
}

void GameScene::Update(){
	// デバックカメラ
	debugCamera_->Update(&viewProjection_); 

	// billBoard
	Matrix4x4 billBroad = MakeBillboard(billBoardTransform_.translation_, viewProjection_.translation_, Vector3(0.0f, 1.0f, 0.0f));
	Matrix4x4 world = MakeAffineMatrix(
		billBoardTransform_.scale_,
		billBoardTransform_.rotation_,
		billBoardTransform_.translation_
	);
	billBoardTransform_.matWorld_ = billBroad * world;
	billBoardTransform_.TransferMatrix();

	// tree
	billBroad = MakeBillboardYAxsizLook(treeWorldTransform_.translation_, viewProjection_.translation_, Vector3(0.0f, 1.0f, 0.0f));
	world = MakeAffineMatrix(
		treeWorldTransform_.scale_,
		treeWorldTransform_.rotation_,
		treeWorldTransform_.translation_
	);
	treeWorldTransform_.matWorld_ = billBroad * world;
	treeWorldTransform_.TransferMatrix();

	if (input_->PushKey(DIK_SPACE)) {
		// ビルボードパーティクル
		billParticle_->SetEmitter(billBoardTransform_.translation_);
		billParticle_->Create();
		// スプライトサンプルパーティクル
 		spriteSampleParticle_->SetEmitter(spriteSampleWorldTransform_.translation_);
		spriteSampleParticle_->Create();
	}

	billParticle_->Update(viewProjection_,true);
	spriteSampleParticle_->Update(viewProjection_,false);
}

void GameScene::Draw(){
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
	Basic::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	base_->Draw(baseWorldTransform_, viewProjection_, textureHandle_1_);
	basicSample_->Draw(basicSampleWorldTransform_,viewProjection_, textureHandle_2_);
	
	// 3Dオブジェクト描画後処理
	Basic::PostDraw();
#pragma endregion
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	spriteSample_->Draw(spriteSampleWorldTransform_, viewProjection_, textureHandle_2_);
	tree_->Draw(treeWorldTransform_, viewProjection_, treeTextureHandle_);
	billBoard_->Draw(billBoardTransform_, viewProjection_,textureHandle_);

	spriteSampleParticle_->Draw(viewProjection_, textureHandle_2_);
	billParticle_->Draw(viewProjection_, textureHandle_);
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void GameScene::Release(){
	SafeDelete(base_);
	SafeDelete(billBoard_);
	SafeDelete(billParticle_);
	SafeDelete(tree_);
	SafeDelete(spriteSample_);
	SafeDelete(spriteSampleParticle_);
	SafeDelete(basicSample_);
	SafeDelete(debugCamera_);
}
