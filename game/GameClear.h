#pragma once
#include "Audio.h"
#include "DebugCamera.h"
#include "FollowCamera.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class DirectXCommon;
class GameClear {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameClear();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameClear();

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
	// 初期設定
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ViewProjection viewProjection_;

	size_t select_SoundHandle_ = 0u;
	size_t gameclear_SoundHandle_ = 0u;

	std::unique_ptr<Sprite> clear_Sprite_;
	uint32_t clear_Sprite_TextureHandle_;
	Vector2 clear_scale_;
	std::unique_ptr<Sprite> pless_b_Sprite_;
	Vector2 pless_b_scale_;
	uint32_t pless_b_Sprite_TextureHandle_;
	bool pless_b_Flag_;
	bool color_flag_;
	uint32_t count_;
	uint32_t count_max_;
	std::unique_ptr<Sprite> black_back_Sprite_;
};