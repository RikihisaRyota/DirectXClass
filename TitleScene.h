#pragma once
#include <memory>

#include "Audio.h"
#include "DebugCamera.h"
#include "FollowCamera.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Player.h"
#include "Skydome.h"
#include "Ground.h"

class DirectXCommon;
class TitleScene {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

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
	
	void Flash(float count);

private: // メンバ変数
#pragma region 初期設定
	     // 初期設定
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ViewProjection viewProjection_;

	std::unique_ptr<Sprite> titie_Sprite_;
	Vector2 titie_scale_;
	uint32_t titie_Sprite_TextureHandle_;
	std::unique_ptr<Sprite> pless_b_Sprite_;
	Vector2 pless_b_scale_;
	uint32_t pless_b_Sprite_TextureHandle_;
	bool pless_b_Flag_;
	bool color_flag_;
	uint32_t count_;
	uint32_t count_max_;

#pragma region プレイヤー
	// プレイヤー
	std::unique_ptr<Player> player_;
	std::unique_ptr<PlayerAttack> playerAttack_;
#pragma endregion
#pragma region 天球
	// 天球
	std::unique_ptr<Skydome> skydome_;
#pragma endregion
#pragma region 地面
	// 地面
	std::unique_ptr<Ground> ground_;
#pragma endregion
#pragma region 追従カメラ
	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
#pragma endregion
};