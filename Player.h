#pragma once
#include <cstdlib>
#include <memory>
#include <optional>

#include "BaseCharacter.h"
#include "Ground.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "input/Input.h"

class PlayerAttack;
class Player : public BaseCharacter, public Collider {
public:
	// ててて
	Vector2 test_pos_ = {1075.0f, 564.0f};
	float distance_X = 61.0f;
	float distance_Y = 30.0f;
	float test_Scale_ = 61.0f;

	Vector2 test_buttan_pos_ = {1094.0f, 616.0f};
	float distance_buttan_X = 13.0f;
	float distance_buttan_Y = 12.0f;
	float test_buttan_Scale_ = 24.0f;

	void SetSpritePos();
	// 体のパーツ
	enum class Parts { 
		HEAD, 
		BODY, 
		ARML, 
		ARMR,
		WEAPON,
		COUNT
	};
	// ふるまい
	enum class Behavior {
		kRoot, // 通常
		kAttack, // 攻撃中
		kDash, // ダッシュ中
	};

	// ダッシュ用ワーク
	struct WorkDash {
		// ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
	};

private:
	WorkDash workDash_;

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::vector<std::unique_ptr<Model>> model) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);
	void DrawUI();
	void HitBoxInitialize() override;
	void HitBoxDraw(const ViewProjection& viewProjection) override;
	/// <summary>
	/// プレイヤーの体の回転
	/// </summary>
	void PlayerRotate(const Vector3& vector);

	void SetSprite(
	    uint32_t chageTextureHandle, uint32_t tripleTextureHandle, uint32_t dashTextureHandle,
	    uint32_t whiteTextureHandle, 
		uint32_t y_buttan_TextureHandle, uint32_t b_buttan_TextureHandle, uint32_t x_buttan_TextureHandle);

	void BehaviorInitialize();

private: // メンバ関数
#pragma region 移動系
	     /// <summary>
	/// ゲームパット入力
	/// </summary>
	void GamePadInput();
	/// <summary>
	/// プレイヤーの移動
	/// </summary>
	void Move();
	/// <summary>
	/// ジャンプ
	/// </summary>
	void Jump();
	/// <summary>
	/// 重力
	/// </summary>
	void Gravity();

	void ChackTranslation();
#pragma endregion
#pragma region Motion系
	/// <summary>
	/// 動き
	/// </summary>
	void Motion();
	/// <summary>
	/// モーションで計算したものを転送
	/// </summary>
	void UpdateMotionMatrix();
	/// <summary>
	/// プレイヤーの体の回転
	/// </summary>
	void PlayerRotate();
	/// <summary>
	/// 浮遊ギミック初期化
	/// </summary>
	void InitializeFloatGimmick();
	/// <summary>
	/// 浮遊ギミック更新
	/// </summary>
	void UpdateFloatGimmick();
	/// <summary>
	/// 全体
	/// </summary>
	void Base();
	/// <summary>
	/// 左腕
	/// </summary>
	void ArmLeft();
	/// <summary>
	/// 右腕
	/// </summary>
	void ArmRight();
	/// <summary>
	/// 頭
	/// </summary>
	void Head();
	/// <summary>
	/// 体
	/// </summary>
	void Body();
#pragma endregion
#pragma region RootBehavior
	/// <summary>
	/// 通常行動初期化
	/// </summary>
	void BehaviorRootInitialize();
	/// <summary>
	///  通常行動更新
	/// </summary>
	void BehaviorRootUpdate();
	/// <summary>
	/// 攻撃行動初期化
	/// </summary>
	void BehaviorAttackInitialize();
	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void BehaviorAttackUpdate();
	/// <summary>
	/// ダッシュ行動初期化
	/// </summary>
	void BehaviorDashInitialize();
	/// <summary>
	/// ダッシュ行動更新
	/// </summary>
	void BehaviorDashUpdate();
#pragma endregion
	void HitBoxUpdate();
	void SetGlobalVariables();
	void GetGlobalVariables();
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const OBB& obb, uint32_t type) override;


	void SpriteUpdate();

public: // ゲッター,セッター
	/// <summary>
	/// ViewProjectionのセッター
	/// </summary>
	/// <param name="viewprojection"></param>
	void SetViewProjection(const ViewProjection* viewprojection) {
		viewProjection_ = viewprojection;
	}
	void SetBehaviorRequest(const std::optional<Behavior>& behaviorRequest) {
		behaviorRequest_ = behaviorRequest;
	}
	void SetBehavior(const Behavior& behavior) {
		behavior_ = behavior;
	}
	void SetPlayerAttack(PlayerAttack* playerAttack) { playerAttack_ = playerAttack; }
	 
	Behavior GetBehavior() const { return behavior_; }

	Vector3 GetPlayerRotate() const { return interRotate_; }

	void SetGround(Ground* ground) { ground_ = ground; }
 private: // 定数系
	// 地面から距離
	const float kGroundDistanse = 1.0f;
	// キャラクターの速さ
	const float kSpeed = 0.15f;
	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
	// 浮遊アニメーションの周期
	const uint16_t kFloatAnimationCycle = 60;
	// 浮遊アニメーションの振幅
	const float kFloatAmplitude = 0.05f;
	// 左腕のアニメーションの振幅
	const float kArmLAmplitude = 0.5f;
	// 右腕のアニメーションの振幅
	const float kArmRAmplitude = 0.5f;
	// 振り向き速度
	const float kTurn = 0.4f;
	// 重力
	const float kGravity = 0.01f;
	// ジャンプ
	const float kJumpPower = 0.2f;
	// ダッシュの時間<frame>
	const uint32_t kDashTime = 15;
	// ダッシュのクールタイム
	const int32_t kDash_CoolTime_ = 30;
	// 摩擦
private: // メンバ変数
	// 最終的に向きたい方向
	Vector3 destinationAngle_;
	// ダッシュ用方向
	Vector3 dashAngle_;
	// ダッシュできるか
	bool IsDash_;
	// ダッシュのクールタイムのカウントフラグ
	bool dash_Count_Start_Flag_;
	// ダッシュのクールタイム
	int32_t dash_Count_;
	// 向き
	Vector3 interRotate_;
	// ジャンプフラグ
	bool isJump;
	// floatアニメーションのカウント
	float floatingParameter_;
	// ふるまい
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	PlayerAttack* playerAttack_;
	Ground* ground_;

	Vector2 chage_Position_;
	Vector2 triple_Position_;
	Vector2 dash_Position_;

	Vector2 y_Position_;
	Vector2 b_Position_;
	Vector2 x_Position_;

	std::unique_ptr<Sprite> chage_Sprite_;
	std::unique_ptr<Sprite> chage_Back_Sprite_;
	std::unique_ptr<Sprite> chage_Back_Black_Sprite_;
	std::unique_ptr<Sprite> triple_Sprite_;
	std::unique_ptr<Sprite> triple_Back_Sprite_;
	std::unique_ptr<Sprite> triple_Back_Black_Sprite_;
	std::unique_ptr<Sprite> dash_Sprite_;
	std::unique_ptr<Sprite> dash_Back_Sprite_;
	std::unique_ptr<Sprite> dash_Back_Black_Sprite_;
	
	std::unique_ptr<Sprite> center_Back_Sprite_;

	
	std::unique_ptr<Sprite> x_Buttan_Sprite_;
	std::unique_ptr<Sprite> b_Buttan_Sprite_;
	std::unique_ptr<Sprite> y_Buttan_Sprite_;
};