#pragma once
#include <cstdlib>
#include "SafeDelete.h"
#include "Input.h"
#include "Basic.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "DebugCamera.h"
#include "BillParticle.h"

class DirectXCommon;
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene
{
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 解放
	/// </summary>
	void Release();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;
	
	/// <summary>
	/// ゲーム用
	/// </summary>
	ViewProjection viewProjection_;

	uint32_t textureHandle_ = 0u;
	uint32_t textureHandle_1_ = 0u;
	uint32_t treeTextureHandle_ = 0u;
	uint32_t textureHandle_2_ = 0u;

	Basic* base_ = nullptr;
	WorldTransform baseWorldTransform_;

	Sprite* billBoard_ = nullptr;
	WorldTransform billBoardTransform_;
	BillParticle* billParticle_ = nullptr;

	Sprite* tree_ = nullptr;
	WorldTransform treeWorldTransform_;

	Sprite* spriteSample_ = nullptr;
	WorldTransform spriteSampleWorldTransform_;
	BillParticle* spriteSampleParticle_ = nullptr;

	Basic* basicSample_ = nullptr;
	WorldTransform basicSampleWorldTransform_;
};

