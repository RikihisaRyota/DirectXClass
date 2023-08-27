#pragma once
#include <optional>

#include "BaseCharacter.h"
#include "Collider.h"

#include "EnemyDash.h"
#include "EnemyMeteo.h"
#include "EnemyPress.h"
#include "EnemyPunch.h"
#include "EnemyTornado.h"

class Enemy;
class Player;
class EnemyAttack : public BaseCharacter, public Collider {
public:
	// ふるまい
	enum class Behavior {
		kRoot,         // 通常
		kPressAttack,  // プレス
		kDashAttack,   // 突進
		kPunchAttack,  // パンチ
		kTornadoAttack, // 回転
		kMeteoAttack, // メテオ
	};

	enum class Parts {
		CIRCLE,
		PLANE,
		METEO,
		COUNT,
	};

public:
	void Initialize(std::vector<std::unique_ptr<Model>> model) override;
	void Initialize();
	void RootInitialize();
	void Update() override;
	void Draw(const ViewProjection& viewProjection);
	void SetPlayerEnemy(Player* player, Enemy* enemy) { player_ = player, enemy_ = enemy; }
	void SetBehavior(const Behavior& behavior) { behaviorRequest_ = behavior; }
	void HitBoxDraw(const ViewProjection& viewProjection) override;
private:
	void HitBoxInitialize() override;
	void HitBoxUpdate() override;
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const OBB& obb, uint32_t type) override;

private:
	Enemy* enemy_;
	Player* player_;

	std::unique_ptr<EnemyPress> press_;
	std::unique_ptr<EnemyDash> dash_;
	std::unique_ptr<EnemyPunch> punch_;
	std::unique_ptr<EnemyTornado> tornade_;
	std::unique_ptr<EnemyMeteo> meteo_;

	// ふるまい
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
};
