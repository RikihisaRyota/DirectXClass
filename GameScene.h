#pragma once
#include <cstdlib>
#include <memory>

#include "Audio.h"
#include "Cube.h"
#include "DebugCamera.h"
#include "Input.h"
#include "Line.h"
#include "Model.h"
#include "OBJ.h"

#include "SafeDelete.h"
#include "Plane.h"
#include "Sphere.h"
#include "Sprite.h"

#include "Block.h"

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
	// ライト
	cDirectionalLight* directionalLight_ = nullptr;

	uint32_t textureHandle_ = 0u;

	/// <summary>
	/// ゲーム用
	/// </summary>
	ViewProjection viewProjection_;
	//std::unique_ptr<Block> block_;

	// 音声再生
	Audio* audio;

	Audio::SoundData soundHandle_;
};