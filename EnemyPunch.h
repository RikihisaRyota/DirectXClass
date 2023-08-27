#pragma once

#include "EnemyAttakType.h"

class EnemyPunch : public EnemyAttackType {
public:
	// パンチ
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
#pragma region ターゲット
	float kTarget_Speed_;
	// 腕の向き
	float kTarget_Start_Radius_;
	float kTarget_End_Radius_;
	float kTarget_Start_Transform_;
	float kTarget_End_Transform_;
	// 体の向き
	Vector3 kTarget_Start_Rotate_;
	Vector3 kTarget_End_Rotate_;
#pragma endregion
#pragma region ステイ
	float kStay_Speed_;
#pragma endregion
#pragma region アタック
	float kAttack_Speed_;
	Vector3 kAttack_Start_Transform_;
	Vector3 kAttack_End_Transform_;
	Vector3 kAttack_Start_Radius_;
	Vector3 kAttack_End_Radius_;
#pragma endregion
	
	float t_;
	WorldTransform target_;
	WorldTransform origin_;

	// どこまで追跡するか
	float target_Distance_;
	State state_;
};
