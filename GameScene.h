#pragma once
#include <cstdlib>
#include <memory>

#include "Basic.h"
#include "DebugCamera.h"
#include "Input.h"
#include "SafeDelete.h"
#include "Sprite.h"

class DirectXCommon;
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {
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
};

