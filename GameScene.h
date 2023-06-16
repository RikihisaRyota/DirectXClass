#pragma once
#include <cstdlib>
#include "SafeDelete.h"
#include "Input.h"
#include "Basic.h"
#include "TextureManager.h"
#include "DebugCamera.h"

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
	WorldTransform worldTransform_;
	WorldTransform worldTransform_2_;

	Basic* triangle_1_;
	Basic* triangle_2_;

	uint32_t textureHandle_ = 0u;
	uint32_t textureHandle_2_ = 0u;
};

