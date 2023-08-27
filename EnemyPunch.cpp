#include "EnemyPunch.h"

#include "Enemy.h"
#include "EnemyAttack.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Player.h"

void EnemyPunch::Initialize() {
	t_ = 0.0f;
	target_ = GetPlayer()->GetWorldTransform();
	origin_ = GetEnemy()->GetWorldTransform();
	target_Distance_ = 5.0f;
	// ヒットボックス
	Vector3 toTarget_ = target_.translation_ - origin_.translation_;
	toTarget_.Normalize();
	toTarget_ = origin_.translation_ + toTarget_ * target_Distance_;
	Vector3 scale{3.0f, 1.0f, 3.0f};
	GetEnemyAttack()->SetScale(scale);
	GetEnemyAttack()->SetTranslation(Vector3(toTarget_.x, 0.01f, toTarget_.z));
#pragma region ターゲット
	kTarget_Speed_ = 0.01f;
	kTarget_Start_Radius_ = GetEnemy()->GetWorldTransforms_Parts(static_cast<int>(Enemy::Parts::ARML)).rotation_.x;
	kTarget_End_Radius_ = DegToRad(-180.0f);
	kTarget_Start_Transform_  = 0.0f;
	kTarget_End_Transform_ = 6.0f;
	// ターゲット時の角度を算出
	// スタートの角度
	Matrix4x4 mat = MakeRotateYMatrix(origin_.rotation_.y);
	kTarget_Start_Rotate_ = TransformNormal(origin_.translation_, mat);
	if (kTarget_Start_Rotate_ != Vector3(0.0f, 0.0f, 0.0f)) {
		kTarget_Start_Rotate_.Normalize();
	}
	// エンドの角度
	kTarget_End_Rotate_ = target_.translation_ - origin_.translation_;
	if (kTarget_End_Rotate_ != Vector3(0.0f, 0.0f, 0.0f)) {
		kTarget_End_Rotate_.Normalize();
	}
#pragma endregion
#pragma region ステイ
	kStay_Speed_ = 0.01f;
#pragma endregion
#pragma region アタック
	kAttack_Speed_ = 0.1f;
	kAttack_Start_Transform_ = GetEnemy()->GetWorldTransforms_Parts(static_cast<int>(Enemy::Parts::ARML)).translation_;
	kAttack_Start_Transform_.y = kTarget_End_Transform_;
	kAttack_End_Transform_ = {0.5f,0.0f,4.0f};
	kAttack_Start_Radius_ = Vector3(kTarget_End_Radius_,0.0f,0.0f);
	kAttack_End_Radius_ = Vector3(0.0f, DegToRad(90.0f), 0.0f);
#pragma endregion
	// 設定
	SetWorking(true);
	SetAttack(false);
	SetHit(false);
	state_ = State::kTarget;
}

void EnemyPunch::Update() {
	switch (state_) {
	case EnemyPunch::State::kRoot:
	default:
		RootUpdate();
		break;
	case EnemyPunch::State::kTarget:
		TargetUpdate();
		break;
	case EnemyPunch::State::kStay:
		StayUpdate();
		break;
	case EnemyPunch::State::kAttack:
		AttackUpdate();
		break;
	}
	ImGui::Begin("enemy_attack ");
	ImGui::Text("t_:%f", t_);
	ImGui::End();
}

void EnemyPunch::RootUpdate() { 
// リセット
	WorldTransform worldTransform = GetEnemy()->GetWorldTransforms_Parts(static_cast<int>(Enemy::Parts::ARML));
	worldTransform.rotation_={0.0f, 0.0f, 0.0f};
	worldTransform.translation_={0.0f, 0.0f, 0.0f};
	GetEnemy()->SetWorldtransforms_Parts(worldTransform, static_cast<int>(Enemy::Parts::ARML));
	SetWorking(false);
}

void EnemyPunch::TargetUpdate() {
	t_ += kTarget_Speed_;
	// プレイヤーの方向に向く
	Vector3 rotate = LenpShortAngle(kTarget_Start_Rotate_, kTarget_End_Rotate_, t_);
	GetEnemy()->EnemyRotate(rotate);
	// 左腕の動き
	WorldTransform worldTransform = GetEnemy()->GetWorldTransforms_Parts(static_cast<int>(Enemy::Parts::ARML));
	worldTransform.rotation_.x = Lerp(kTarget_Start_Radius_, kTarget_End_Radius_, Clamp(t_, 0.0f, 1.0f));
	worldTransform.translation_.y = Lerp(kTarget_Start_Transform_, kTarget_End_Transform_, Clamp(t_, 0.0f, 1.0f));
	GetEnemy()->SetWorldtransforms_Parts(worldTransform, static_cast<int>(Enemy::Parts::ARML));
	if (t_ >= 1.0f) {		
		state_ = State::kStay;
		t_ = 0.0f;
	}
}

void EnemyPunch::StayUpdate() {
	t_ += kStay_Speed_;
	
	if (t_ >= 1.0f) {
		state_ = State::kAttack;
		t_ = 0.0f;
	}
}

void EnemyPunch::AttackUpdate() {
	t_ += kAttack_Speed_;
	// 左腕の動き
	WorldTransform worldTransform = GetEnemy()->GetWorldTransforms_Parts(static_cast<int>(Enemy::Parts::ARML));
	worldTransform.rotation_ = Lerp(kAttack_Start_Radius_, kAttack_End_Radius_, Clamp(t_, 0.0f, 1.0f));
	worldTransform.translation_ = Lerp(kAttack_Start_Transform_, kAttack_End_Transform_, Clamp(t_, 0.0f, 1.0f));
	GetEnemy()->SetWorldtransforms_Parts(worldTransform, static_cast<int>(Enemy::Parts::ARML));
	if (t_ >= 0.8) {
		UpdateHit();
	}
	if (t_ >= 1.5f) {
		state_ = State::kRoot;
		t_ = 0.0f;
	}
}
