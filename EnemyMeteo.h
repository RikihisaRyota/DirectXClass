#pragma once

#include "EnemyAttakType.h"

class EnemyMeteo : public EnemyAttackType {
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
	const size_t kMeteoCount = 15;
	const Vector3 kScale_ = {5.0f, 1.0f, 5.0f};
	const float kHeight = 15.0f;
#pragma region ターゲット
	float kTarget_Speed_;
	size_t target_Count;
#pragma endregion
#pragma region ステイ
	float kStay_Speed_;
#pragma endregion
#pragma region アタック
	float kAttack_Speed_;
#pragma endregion
	float t_;
	WorldTransform target_;
	WorldTransform origin_;
	State state_;
};
