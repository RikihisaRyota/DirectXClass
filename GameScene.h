#pragma once
#include <cstdlib>
#include <memory>

#include "Audio.h"
#include "Cube.h"
#include "DebugCamera.h"
#include "FollowCamera.h"
#include "Input.h"
#include "PrimitiveDrawer.h"
#include "Model.h"
#include "OBJ.h"
#include "SafeDelete.h"
#include "PlaneRenderer.h"
#include "SphereRenderer.h"
#include "Sprite.h"

#include "Block.h"
#include "CollisionManager.h"
#include "Enemy.h"
#include "EnemyAttack.h"
#include "Player.h"
#include "Skydome.h"

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
	FollowCamera followCamera_;

	std::unique_ptr<Block> block_;
	std::unique_ptr<CollisionManager> collisionManager_;
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<EnemyAttack> enemyAttack_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Skydome> skydome_;
};