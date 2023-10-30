#pragma once

#include "EnemyAttakType.h"

class EnemyTornado : public EnemyAttackType {
public:
	// プレス
	enum class State {
		kRoot,
		kTarget,
		kStay,
		kAttack,
	};

public:
	void Initialize() override;
	void Update() override;

private:
	void RootUpdate();
	void StayUpdate();
	void TargetUpdate();
	void AttackUpdate();

private:
	float t_;
	float kTarget_Speed_;
	float kStay_Speed_;
	float kAttack_Speed_;
	WorldTransform target_;
	WorldTransform origin_;
	State state_;
};
