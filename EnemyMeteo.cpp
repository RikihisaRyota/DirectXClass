#include "EnemyMeteo.h"

#include "Enemy.h"
#include "EnemyAttack.h"
#include "Ground.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Player.h"
#include "RandomNumberGenerator.h"

#include "ImGuiManager.h"

void EnemyMeteo::Initialize() {
	t_ = 0.0f;
	origin_ = GetEnemy()->GetWorldTransform();
	target_ = GetPlayer()->GetWorldTransform();
	GetEnemyAttack()->SetScale(kScale_,0);
	// 最初のポジションだけ決める
	RandomNumberGenerator rnd;
	float x = rnd.NextFloatRange(-Ground::GetEdge() + kScale_.x, Ground::GetEdge() - kScale_.x);
	float z = rnd.NextFloatRange(-Ground::GetEdge() + kScale_.x, Ground::GetEdge() - kScale_.z);
	GetEnemyAttack()->SetTranslation(Vector3(x,0.01f,z),0);
	WorldTransform part = GetEnemyAttack()->GetWorldTransforms_Parts(static_cast<int>(EnemyAttack::Parts::METEO),0);
	part.scale_.y = 5.0f;
	part.translation_.y = kHeight;
	GetEnemyAttack()->SetWorldtransforms_Parts(part, static_cast<int>(EnemyAttack::Parts::METEO),0);
#pragma region ターゲット
	target_Count = 1;
	kTarget_Speed_ = 0.01f;
#pragma endregion
#pragma region ステイ
	kStay_Speed_ = 0.05f;
#pragma endregion
#pragma region アタック
	kAttack_Speed_ = 0.02f;
#pragma endregion

	// 設定
	SetWorking(true);
	SetAttack(false);
	SetHit(false);
	state_ = EnemyMeteo::State::kTarget;
}

void EnemyMeteo::Update() {
	switch (state_) {
	case EnemyMeteo::State::kRoot:
	default:
		RootUpdate();
		break;
	case EnemyMeteo::State::kTarget:
		TargetUpdate();
		break;
	case EnemyMeteo::State::kStay:
		StayUpdate();
		break;
	case EnemyMeteo::State::kAttack:
		AttackUpdate();
		break;
	}
	ImGui::Begin("meteo");
	for (size_t i = 0; i < target_Count; i++) {
		Vector3 transform = GetEnemyAttack()->GetWorldTransform(i).translation_;
		ImGui::Text("%d:transform:%f,%f,%f",i, transform.x, transform.y, transform.x);
	}
	ImGui::End();
}

void EnemyMeteo::RootUpdate() { SetWorking(false); }

void EnemyMeteo::TargetUpdate() {
	t_ += kTarget_Speed_;
	const float kInterval = target_Count / static_cast<float>(kMeteoCount);
	if (t_ > kInterval) {
		RandomNumberGenerator rnd;
		float distance;
		Vector3 dest;
	while (true) {
			float x =
			    rnd.NextFloatRange(-Ground::GetEdge() + kScale_.x, Ground::GetEdge() - kScale_.x);
			float z =
			    rnd.NextFloatRange(-Ground::GetEdge() + kScale_.x, Ground::GetEdge() - kScale_.z);
			dest = {x, 0.01f, z};
			for (size_t i = 0; i < target_Count; i++) {
				WorldTransform src = GetEnemyAttack()->GetWorldTransform(i);
				distance = Length(src.translation_ + dest);
				if (distance < kScale_.x * 2.0f) {
					continue;
				}
			}
			break;
		}
		// ワールドトランスフォームの追加
		WorldTransform world;
		world.Initialize();
		world.scale_ = kScale_;
		world.translation_= dest;
		GetEnemyAttack()->AddWorldtransform(world);
		// メテオの位置調整
		WorldTransform part = GetEnemyAttack()->GetWorldTransforms_Parts(static_cast<int>(EnemyAttack::Parts::METEO), target_Count);
		part.scale_.y = 5.0f;
		part.rotation_.y = DegToRad(rnd.NextFloatRange(0.0f, 180.0f));
		part.translation_.y = kHeight;
		GetEnemyAttack()->SetWorldtransforms_Parts(part, static_cast<int>(EnemyAttack::Parts::METEO), target_Count);
		target_Count++;
	}
	if (t_ >= 1.0f) {
		state_ = State::kStay;
		t_ = 0.0f;
	}
}

void EnemyMeteo::StayUpdate() {
	t_ += kStay_Speed_;

	if (t_ >= 1.0f) {
		state_ = State::kAttack;
		t_ = 0.0f;
	}
}

void EnemyMeteo::AttackUpdate() {
	t_ += kAttack_Speed_;
	
	for (size_t i = 0; i < GetEnemyAttack()->GetWorldtransformSize(); i++) {
		WorldTransform part = GetEnemyAttack()->GetWorldTransforms_Parts(
		    static_cast<int>(EnemyAttack::Parts::METEO), i);
		part.translation_.y = Lerp(kHeight, kScale_.x, t_);
		GetEnemyAttack()->SetWorldtransforms_Parts(part, static_cast<int>(EnemyAttack::Parts::METEO), i);
	}
	if (t_ >= 0.8) {
		UpdateHit();
	}
	if (t_ >= 1.5f) {
		state_ = State::kRoot;
		t_ = 0.0f;
	}
}