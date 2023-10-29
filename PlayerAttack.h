#pragma once
#include <optional>
#include <list>

#include "BaseCharacter.h"
#include "Collider.h"
#include "Plate.h"

class Player;
class Enemy;
class PlayerAttack : public BaseCharacter, public Collider {
public:
	enum class Parts {
		WEAPON,
		COUNT,
	};

	// ふるまい
	enum class Behavior {
		kRoot, // 通常
		kChargeAttack, // 攻撃中
		kTripleAttack,   // ３段攻撃中
	};

	enum class TripleAttack {
		kRoot,
		kFirst,
		kSecond,
		kThird,
	};

	struct Particle {
		WorldTransform worldTransform_;
		Vector3 velocity_;
		int time_;
		bool IsAlive_;
		Plate* plate_;
	};

	struct HitParticle {
		WorldTransform worldTransform_;
		Vector3 velocity_;
		int time_;
		bool IsAlive_;
		Model* model_;
	};

public:
	/// <summary>
	/// 初期化（モデル代入）
	/// </summary>
	void Initialize(std::vector<std::unique_ptr<Model>> model) override;
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);
	void ParticleDraw(const ViewProjection& viewProjection);
	void HitBoxInitialize(uint32_t collisionMask) override;
	void HitBoxDraw(const ViewProjection& viewProjection) override;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const OBB& obb, const WorldTransform& worldTransform, uint32_t type) override;

	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemy(Enemy* enmey) { enemy_ =enmey; }
	void SetBehavior(const Behavior& behavior) { behaviorRequest_ = behavior; }

	int32_t GetChageAttackCount() { return kChageAttackCount; }
	int32_t GetChageAttackCoolTimrCount() { return kChageAttackCoolTime; }

	int32_t GetTripleAttackCount() { return kTripleAttackCount; }
	int32_t GetTripleCoolTimeAttackCount() { 
	if (IsTripleAttack_) {
			return kTripleAttack_Count;
		} 
		return kTripleAttackCoolTime;
		
	}
	bool GetChageAttackFlag() { return IsChageAttack_; }
	bool GetTripleAttackFlag() { return IsTripleAttack_; }

	void ParticleRelease();

private:
	void ChageAttackInitialize();
	void TripleAttackInitialize();
	void RootUpdate();
	void ChageAttackUpdate();
	void TripleAttackUpdate();

	void FirstInitialize();
	void SecondInitialize();
	void ThirdInitialize();
	void FirstUpdate();
	void SecondUpdate();
	void ThirdUpdate();

	void HitBoxUpdate() override;
	void Homing();

	void ChargeParticleCreate(const Vector3& emitter);
	void ChargeParticleUpdate();
	void HitParticleCreate(const Vector3& emitter);
	void HitParticleUpdate();
	void ParticleUpdate();
	
	void ChargeParticleDraw(const ViewProjection& viewProjection);
	void HitParticleDraw(const ViewProjection& viewProjection);
private:
	Player* player_;
	Enemy* enemy_;

	// ふるまい
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Vector3 tripleAttackMin_;
	Vector3 tripleAttackMax_;
	Vector3 tripleAttackSize_;
	Vector3 center_Distance_;

	bool hitFlag_ = false;

	// 剣のデットゾーン
	float slashMin_;
	float slashMax_;
	float slash_Attack_Min_;
	float slash_Attack_Max_;
	// 初めの角度
	float slash_Attack_Start_;
	float slash_ArmAngle_Start_;
	// 攻撃の溜めモーションスピード
	float charge_Speed_;
	float charge_T_;
	// 溜めているかどうかのフラグ
	bool chargeFlag_;
	// 降り下ろしモーション
	float slash_Speed_;
	float slash_T_;
	// 溜めてあと立てるフラグ
	bool slashFlag_;
	// 攻撃硬直
	bool rigorFlag_;
	float rigor_Speed_;
	float rigor_T_;


	// ３段攻撃
	float t_;
	// 一回目
	float first_Speed_;
	float armAngleStart_;
	float armAngleMax_;
	float armSlideStart_;
	float armSlideMax_;
	float bodyAngleStart_;
	float bodyAngleMax_;
	// ２回目
	float second_Speed_;
	// 3回目
	float third_Speed_;
	
	// 三段攻撃
	TripleAttack tripleAttack_Behavior_ = TripleAttack::kRoot;

	bool IsChageAttack_ = true;
	bool ISChageAttack_Count_Start_ = false;
	bool IsTripleAttack_ = true;
	bool ISTripleAttack_Count_Start_ = false;

	const int32_t kChageAttackCoolTime = 360;
	int32_t kChageAttackCount;
	const int32_t kTripleAttackCoolTime = 60;
	const int32_t kTripleAttack_Count = 120;
	int32_t kTripleAttackCount;

	std::list<std::unique_ptr<Particle>> chage_Particles_;
	std::list<std::unique_ptr<Particle>> hit_Particles_;
	uint32_t particle_Count_;
	uint32_t particle_Cooltime_ = 3;
	float distance_min_ = 5.0f;
	float distance_max_ = 10.0f;
	float scale_ = 0.1f;
	float max_scale_ = 1.0f;
	uint32_t time_min_ = 5;
	uint32_t time_max_ = 10;

	size_t	first_SoundHandle_;
	size_t	third_SoundHandle_;
	size_t	chage_SoundHandle_;
};
