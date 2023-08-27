#include "EnemyTornado.h"

#include "Enemy.h"
#include "EnemyAttack.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Player.h"

void EnemyTornado::Initialize() {
	t_ = 0.0f;
	kTarget_Speed_ = 0.05f;
	kStay_Speed_ = 0.05f;
	kAttack_Speed_ = 0.05f;
	origin_ = GetEnemy()->GetWorldTransform();
	target_ = origin_;
	target_.scale_ = {1.2f, 0.8f, 1.2f};

	// 落下地点をセット
	const float scale = 6.0f;
	GetEnemyAttack()->SetScale(Vector3(scale, 1.0f, scale));
	GetEnemyAttack()->SetTranslation(
	    Vector3(origin_.translation_.x, 0.01f, origin_.translation_.z));
	// 設定
	SetWorking(true);
	SetAttack(false);
	SetHit(false);
	state_ = EnemyTornado::State::kTarget;
}

void EnemyTornado::Update() {
	switch (state_) {
	case EnemyTornado::State::kRoot:
	default:
		RootUpdate();
		break;
	case EnemyTornado::State::kTarget:
		TargetUpdate();
		break;
	case EnemyTornado::State::kStay:
		StayUpdate();
		break;
	case EnemyTornado::State::kAttack:
		AttackUpdate();
		break;
	}
}

void EnemyTornado::RootUpdate() { SetWorking(false); }

void EnemyTornado::TargetUpdate() {
	t_ += kTarget_Speed_;

	Vector3 scale = Lerp(origin_.scale_, target_.scale_, Clamp(t_, 0.0f, 1.0f));
	GetEnemy()->SetScale(scale);
	if (t_ >= 1.0f) {
		state_ = State::kStay;
		t_ = 0.0f;
	}
}

void EnemyTornado::StayUpdate() {
	t_ += kStay_Speed_;

	if (t_ >= 1.0f) {
		state_ = State::kAttack;
		t_ = 0.0f;
	}
}

void EnemyTornado::AttackUpdate() {
	t_ += kAttack_Speed_;
	// 回転	
	
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;

	double t = t_ < 0.5 ? (std::pow(2 * t_, 2) * ((c2 + 1) * 2 * t_ - c2)) / 2
	               : (std::pow(2 * t_ - 2, 2) * ((c2 + 1) * (t_ * 2 - 2) + c2) + 2) / 2;

	Vector3 rotate{0.0f,0.0f,0.0f};
	rotate.y = Lerp(DegToRad(0.0f), DegToRad(720.0f), static_cast<float>(t));
	GetEnemy()->SetRotation(rotate);
	// スケール
	Vector3 scale = Lerp(target_.scale_, origin_.scale_, Clamp(t_,0.0f,1.0f));
	GetEnemy()->SetScale(scale);
	if (t_ >= 0.8) {
		UpdateHit();
	}
	if (t_ >= 1.5f) {
		state_ = State::kRoot;
		t_ = 0.0f;
	}
}