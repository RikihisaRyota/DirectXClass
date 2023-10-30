#pragma once
#include <optional>
#include "Collider.h"
#include "EnemyAttack.h"
class Player;
class Enemy : public Collider {
public:
	// ふるまい
	enum class Behavior {
		kRoot,   // 通常
		kAttack, // 攻撃中
	};

	// パーツ
	enum class Parts { 
		BODY, 
		HEAD,
		ARML,
		ARMR,
		COUNT
	};
public:
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

	void HitBoxDraw(const ViewProjection& viewProjection) override;
	void SetBehavior(const std::optional<Behavior>& behaviorRequest) {
		behaviorRequest_ = behaviorRequest;
	}
	Behavior GetBehavior() const { return behavior_; }
	void SetEnemyAttack(EnemyAttack* enemyAttack) { enemyAttack_ = enemyAttack; }
	void EnemyRotate(const Vector3& vector);
	float GetFloorDistance() { return kFloor_Distance_; }
	void SetPlayer(Player* player) { player_ = player; }
	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool flag) { isAlive_ = flag; }
private:
	void HitBoxInitialize(uint32_t collisionMask) override;
	void RootInitialize();
	void RootUpdate();
	/// <summary>
	/// 移動
	/// </summary>
	void Move();
	/// <summary>
	/// 動作
	/// </summary>
	void Motion();
	/// <summary>
	/// 全体
	/// </summary>
	void Base();
	/// <summary>
	/// 体
	/// </summary>
	void Body();

	void HitBoxUpdate() override;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const OBB& obb, const WorldTransform& worldTransform, uint32_t type) override;
private:
	// 振り向き速度
	const float kTurn = 0.4f;

	const float kFloor_Distance_ = 2.5f;

	const float kMove_Speed_ = 0.05f;

	const float kDistance_ = 5.0f;
	const float kDash_Distance_ = 10.0f;
private:
	AABB area_;
	Player* player_;
	// 向き
	Vector3 interRotate_;
	// さいしゅうてきに向きたい方向
	Vector3 destinationAngle_;
	// 移動回転用
	float moveRatate_;
	// 動作回転用
	float motionRatate_;
	// ふるまい
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	// 攻撃
	EnemyAttack* enemyAttack_;
	/////////
	float angle_ = 0.0f;

	bool dash_Attack_;

	bool triple_Attack_;
	EnemyAttack::Behavior triple_Attack_State_;
	uint32_t triple_count_ = 0;
	uint32_t triple_count_Max = 3;

	bool meteo_Attack_;
	uint32_t meteo_count_ = 1;
	uint32_t meteo_count_Max = 4;

	bool Isbreak_;
	uint32_t break_count_ = 0;
	uint32_t break_count_Max = 30;

	bool isAlive_;
};
