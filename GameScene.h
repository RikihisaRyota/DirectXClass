#pragma once
#include <cstdlib>
#include <memory>

#include "Audio.h"
#include "CubeRenderer.h"
#include "DebugCamera.h"
#include "Input.h"
#include "PrimitiveDrawer.h"
#include "Model.h"
#include "OBJ.h"
#include "Pera.h"
#include "SafeDelete.h"
#include "PlaneRenderer.h"
#include "SphereRenderer.h"
#include "Sprite.h"

#include "ICommand.h"
#include "InputHandler.h"
#include "Player.h"

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
	// 音声再生
	Audio* audio_;
	/// <summary>
	/// ゲーム用
	/// </summary>
	ViewProjection viewProjection_;

	ICommand* command_;
	std::unique_ptr<InputHandler> inputHandler_;
	std::unique_ptr<Player> player_;

};