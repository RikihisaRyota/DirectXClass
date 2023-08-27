#pragma once

#include "EnemyAttakType.h"

class EnemyDash : public EnemyAttackType {
public:
	// プレス
	enum class State {
		kRoot,
		kTarget,
		kAttack,
	};

public:
	void Initialize() override;
	void Update() override;

private:
	void RootUpdate();
	void TargetUpdate();
	void DrapUpdate();

private:
	float t_;
	float kTarget_Speed_;
	Vector3 kTarget_Start_Rotate_;
	Vector3 kTarget_End_Rotate_;
	float kDash_Speed_;
	WorldTransform target_;
	WorldTransform origin_;
	Vector3 toTarget_;
	// どこまで追跡するか
	float target_Distance_;
	State state_;
};