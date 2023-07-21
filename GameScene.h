#pragma once
#include <cstdlib>
#include <memory>

#include "Basic.h"
#include "Cube.h"
#include "DebugCamera.h"
#include "Input.h"
#include "OBJ.h"
#include "Pera.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "Sphere.h"

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
	DirectionalLight* directionalLight_ = nullptr;
	std::unique_ptr<Sphere> light_;
	WorldTransform lightWorldTransform_;

	std::unique_ptr<Sphere> origin_;
	WorldTransform originWorldTransform_;

	std::array<bool, 2> IsCamera_;

	uint32_t textureHandle_ = 0u;
	/// <summary>
	/// ゲーム用
	/// </summary>
	ViewProjection viewProjection_;
	// 土台
	std::unique_ptr<Basic> base_;
	WorldTransform baseWorldTransform_;
	// obj
	std::unique_ptr<OBJ> obj_;
	WorldTransform objWorldTransform_;
	// cube
	std::unique_ptr<Cube> cube_;
	WorldTransform cubeWorldTransform_;
	// sphere
	std::unique_ptr<Sphere> sphere_;
	WorldTransform sphereWorldTransform_;
	// Pera
	std::unique_ptr<Pera> pera_;
	// sprite
	static const int32_t kCount = 2;
	std::array<std::unique_ptr<Sprite>,kCount> sprite_;
	std::array<WorldTransform,kCount> spriteWorldTransform_;
	std::array<Vector4,kCount> sprite_Color_;
	std::array<bool,kCount>isTexture_;
	std::array<uint32_t ,kCount> spriteTextureHandle_;
};

