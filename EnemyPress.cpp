#include "EnemyPress.h"

#include "Enemy.h"
#include "EnemyAttack.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Player.h"

void EnemyPress::Initialize() {
	t_ = 0.0f;
	kTarget_Speed_ = 0.05f;
	kStay_Speed_ = 0.01f;
	kAttack_Speed_ = 0.05f;
	target_ = GetPlayer()->GetWorldTransform();
	origin_ = GetEnemy()->GetWorldTransform();
	// 落下地点をセット
	const float scale = 5.0f;
	GetEnemyAttack()->SetScale(Vector3(scale, 1.0f, scale));
	GetEnemyAttack()->SetTranslation(Vector3(target_.translation_.x, 0.01f, target_.translation_.z));
	// 上空にするため
	const float Up = 10.0f;
	target_.translation_.y += Up;
	// 設定
	SetWorking(true);
	SetAttack(false);
	SetHit(false);
	state_ = EnemyPress::State::kTarget;
}

void EnemyPress::Update() {
	switch (state_) {
	case EnemyPress::State::kRoot:
	default:
		RootUpdate();
		break;
	case EnemyPress::State::kTarget:
		TargetUpdate();
		break;
	case EnemyPress::State::kStay:
		StayUpdate();
		break;
	case EnemyPress::State::kAttack:
		AttackUpdate();
		break;
	}
	ImGui::Begin("enemy_attack ");
	ImGui::Text("t_:%f", t_);
	ImGui::End();
}

void EnemyPress::RootUpdate() { 
	SetWorking(false); 
}

void EnemyPress::TargetUpdate() {
	t_ += kTarget_Speed_;

	Vector3 move = Lerp(origin_.translation_, target_.translation_, Clamp(t_, 0.0f, 1.0f));
	GetEnemy()->SetTranslation(move);
	Vector3 vector = target_.translation_ - origin_.translation_;
	GetEnemy()->EnemyRotate(vector);
	if (t_ >= 1.0f) {
		state_ = State::kStay;
		t_ = 0.0f;
	}
	
}

void EnemyPress::StayUpdate() { 
	t_ += kStay_Speed_; 
	WorldTransform world = GetEnemy()->GetWorldTransform();
	world.rotation_.y += 0.5;
	
	Vector3 rotate{};
	float T = std::sin((t_ * static_cast<float>(PI)) * 0.5f);
	rotate.y = Lerp(origin_.rotation_.y, origin_.rotation_.y + DegToRad(720.0f), T);
	GetEnemy()->SetRotation(rotate);
	if (t_ >= 1.0f) {
		origin_ = target_;
		state_ = State::kAttack;
		t_ = 0.0f;
	}
}

void EnemyPress::AttackUpdate() {
	t_ += kAttack_Speed_;
	origin_.translation_.y = Lerp(origin_.translation_.y, GetEnemy()->GetFloorDistance(), Clamp(t_, 0.0f, 1.0f));
	GetEnemy()->SetWorldtransform(origin_);
	if (t_ >= 0.8) {
		UpdateHit();
	}
	if (t_ >= 1.5f) {
		state_ = State::kRoot;
		t_ = 0.0f;
	}
}