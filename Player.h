#pragma once
#include <cstdlib>
#include <memory>
#include <optional>

#include "Collider.h"
#include "BaseCharacter.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "input.h"

class Enemy;
class EnemyAttack;
class PlayerAttack;
class Player : public Collider {
public:
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
	void Initialize(std::vector<Model*> model) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection)override;
	void HitBoxInitialize(uint32_t collisionMask) override;
	void HitBoxDraw(const ViewProjection& viewProjection) override;
	/// <summary>
	/// プレイヤーの体の回転
	/// </summary>
	void PlayerRotate(const Vector3& vector);

	void BehaviorInitialize();
	void SetGlobalVariables();
	void GetGlobalVariables();
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
	void HitBoxUpdate()override;
	
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const OBB& obb, const WorldTransform& worldTransform, uint32_t type) override;

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

	Behavior GetBehavior() const { return behavior_; }

	Vector3 GetPlayerRotate() const { return interRotate_; }

	void SetPlayerAttack(PlayerAttack* playerAttack) { playerAttack_ = playerAttack; }
	void SetEnemyAttack(std::vector<EnemyAttack*> enemyAttack) { enemyAttack_ = enemyAttack; }
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
	int32_t kDashTime=15;
	// ダッシュのクールタイム
	const int32_t kDash_CoolTime_ = 30;
	// 摩擦
private: // メンバ変数
	PlayerAttack* playerAttack_;
	std::vector<EnemyAttack*>enemyAttack_;
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

};