#include "EnemyDash.h"

#include "Enemy.h"
#include "EnemyAttack.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Player.h"

void EnemyDash::Initialize() { 
	t_ = 0.0f;
	kTarget_Speed_ = 0.02f;
	kDash_Speed_ = 0.01f;
	
	target_ = GetPlayer()->GetWorldTransform();
	origin_ = GetEnemy()->GetWorldTransform();
	toTarget_ = target_.translation_ - origin_.translation_;
	toTarget_.y = 0.0f;
	toTarget_.Normalize();
	target_Distance_ = 25.0f;
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
	// 落下地点をセット
	GetEnemyAttack()->SetWorldtransform(GetEnemy()->GetWorldTransform());
	WorldTransform worldTransform = GetEnemyAttack()->GetWorldTransform();
	// スケール
	const Vector3 kScale{2.0f, 1.0f, target_Distance_*0.5f};
	// 角度
	Vector3 rotate = target_.translation_ - origin_.translation_;
	rotate.y = std::atan2(rotate.x, rotate.z);
	rotate.x = 0.0f;
	rotate.z = 0.0f;
	// 移動
	Vector3 transform = origin_.translation_ + toTarget_ * target_Distance_ * 0.5f;
	transform.y = 0.01f;
	// 代入
	worldTransform.scale_ = kScale;
	worldTransform.rotation_ = rotate; 
	worldTransform.translation_ = transform; 
	GetEnemyAttack()->SetWorldtransform(worldTransform);

	// 設定
	SetWorking(true);
	SetAttack(false);
	SetHit(false);
	state_ = State::kTarget;
}

void EnemyDash::Update() {
	switch (state_) {
	case EnemyDash::State::kRoot:
	default:
		RootUpdate();
		break;
	case EnemyDash::State::kTarget:
		TargetUpdate();
		break;
	case EnemyDash::State::kAttack:
		DrapUpdate();
		break;
	}
	ImGui::Begin("Dash");
	ImGui::Text("T:%f", t_);
	ImGui::End();
}

void EnemyDash::RootUpdate() { 
	SetWorking(false);
}

void EnemyDash::TargetUpdate() {
	t_ += kTarget_Speed_;
	
	Vector3 rotate = LenpShortAngle(kTarget_Start_Rotate_, kTarget_End_Rotate_, t_);
	GetEnemy()->EnemyRotate(rotate);
	if (t_ >= 1.0f) {
		state_ = State::kAttack;
		t_ = 0.0f;
	}
}

void EnemyDash::DrapUpdate() {
	t_ += kDash_Speed_;
	Vector3 move = Lerp(
	    origin_.translation_, origin_.translation_ + toTarget_ * target_Distance_,
	    Clamp(t_, 0.0f, 1.0f));
	GetEnemy()->SetTranslation(move);
	UpdateHit();
	if (t_ >= 1.5f) {
		state_ = State::kRoot;
		t_ = 0.0f;
	}
}
