#include "Enemy.h"

#include"Draw.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "MyMath.h"
#include "EnemyAttack.h"
#include "Player.h"

#include "RandomNumberGenerator.h"

void Enemy::Initialize(std::vector<std::unique_ptr<Model>> model) {
	BaseCharacter::Initialize(std::move(model));
	// worldTransform_をずらす
	worldTransform_.at(0).translation_ = Vector3(0.0f, kFloor_Distance_, 10.0f);
	vector_ = Normalize(worldTransform_.at(0).translation_);
	// AABBのサイズ
	AABB aabb;
	OBB obb;
	aabb_.emplace_back(aabb);
	obb_.emplace_back(obb);
	// 変数初期化
	interRotate_ = worldTransform_.at(0).rotation_;
	moveRatate_ = 0.0f;
	motionRatate_ = 0.0f;
	// 転送
	BaseCharacter::Update();
#pragma region 当たり判定
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

	HitBoxInitialize();
#pragma endregion
}

void Enemy::Update() {
	if (behavior_ != Enemy::Behavior::kAttack && Input::GetInstance()->PushKey(DIK_1)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPressAttack);
	}
	if (behavior_ != Enemy::Behavior::kAttack && Input::GetInstance()->PushKey(DIK_2)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kDashAttack);
	}
	if (behavior_ != Enemy::Behavior::kAttack && Input::GetInstance()->PushKey(DIK_3)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPunchAttack);
	}
	if (behaviorRequest_) {
		// ふるまいを変更
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Enemy::Behavior::kRoot:
		default:
			RootInitialize();
			break;
		case Enemy::Behavior::kAttack:
			enemyAttack_->Initialize();
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_) {
	case Enemy::Behavior::kRoot:
	default:
		RootUpdate();
		break;
	case Enemy::Behavior::kAttack:
		enemyAttack_->Update();
		break;
	}
	// 転送
	BaseCharacter::Update();
	HitBoxUpdate();
//#ifdef DEBUG
	ImGui::Begin("Enemy");
	ImGui::Text(
	    "translation_ x:%f,y:%f,z:%f", worldTransform_.at(0).translation_.x,
	    worldTransform_.at(0).translation_.y, worldTransform_.at(0).translation_.z);
	ImGui::Text(
	    "rotate_ x:%f,y:%f,z:%f", worldTransform_.at(0).rotation_.x,
	    worldTransform_.at(0).rotation_.y,
	    worldTransform_.at(0).rotation_.z);
	ImGui::Text("vector_x:%f,y:%f,z:%f", vector_.x, vector_.y, vector_.z);
	ImGui::Text("velocity_:%f,y:%f,z:%f", velocity_.x, velocity_.y, velocity_.z);
	ImGui::Text("acceleration_:%f,y:%f,z:%f", acceleration_.x, acceleration_.y, acceleration_.z);
	ImGui::End();
//#endif // DEBUG
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	for (size_t i = 0; i < worldTransforms_Parts_.at(0).size(); i++) {
		models_[i]->Draw(worldTransforms_Parts_.at(0)[i], viewProjection);
	}
}

void Enemy::EnemyRotate(const Vector3& vector1) {
	Vector3 vector =vector1;
	if (vector != Vector3(0.0f, 0.0f, 0.0f)) {
		vector.Normalize();
	}
	if (interRotate_ != Vector3(0.0f, 0.0f, 0.0f)) {
		interRotate_.Normalize();
	}
	Vector3 rotate = Lerp(interRotate_, vector, kTurn);
	//  Y軸回り角度(θy)
	worldTransform_.at(0).rotation_.y = std::atan2(rotate.x, rotate.z);
	// プレイヤーの向いている方向
	interRotate_ = rotate;
}

void Enemy::HitBoxInitialize() {
	// AABB
	min_ = {-5.0f, -5.0f, -5.0f};
	max_ = {5.0f, 5.0f, 5.0f};
	// OBB
	size_ = {2.5f, 5.0f, 2.5f};
	// Sphere
	radius_ = 1.2f;
	// AABB
	aabb_.at(0) = {
	    .center_{worldTransform_.at(0).translation_},
	    .min_{aabb_.at(0).center_ + min_},
	    .max_{aabb_.at(0).center_ + max_},
	};
	// OBB
	obb_.at(0) = {
	    .center_{worldTransform_.at(0).translation_},
	    .orientations_{
	             {1.0f, 0.0f, 0.0f},
	             {0.0f, 1.0f, 0.0f},
	             {0.0f, 0.0f, 1.0f},
	             },
	    .size_{size_}
    };
	obb_.at(0) = OBBSetRotate(obb_.at(0), worldTransform_.at(0).rotation_);
	// Sphere
	sphere_ = {
	    .center_{worldTransform_.at(0).translation_},
	    .radius_{radius_},
	};
}

void Enemy::HitBoxDraw(const ViewProjection& viewProjection) {
	DrawAABB(aabb_.at(0), viewProjection, Vector4(0.0f, 0.5f, 0.25f, 1.0f));
	DrawOBB(obb_.at(0), viewProjection, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void Enemy::RootInitialize() {
	// 変数初期化
	worldTransform_.at(0).translation_.y = kFloor_Distance_;
	BaseCharacter::Update();
	moveRatate_ = 0.0f;
	motionRatate_ = 0.0f;
}

void Enemy::RootUpdate() {
	RandomNumberGenerator rnd;
	/*int i = rnd.NextUIntRange(0, 2);
	if (i == 0) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPressAttack);
	} else if (i == 1) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kDashAttack);
	} else {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPunchAttack);
	}*/
	
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPressAttack);
	}
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kDashAttack);
	} 
	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPunchAttack);
	}
	if (Input::GetInstance()->TriggerKey(DIK_4)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kTornadoAttack);
	}
	if (Input::GetInstance()->TriggerKey(DIK_5)) {
		behaviorRequest_ = Behavior::kAttack;
		enemyAttack_->SetBehavior(EnemyAttack::Behavior::kMeteoAttack);
	}
	if (std::fabs(Length(worldTransform_.at(0).translation_ - player_->GetWorldTransform().translation_))< kDistance_) {
		RandomNumberGenerator rnd;
		int type = rnd.NextUIntRange(0,static_cast<int>(EnemyAttack::Behavior::kCount) - 1);
		switch (type) {
		case static_cast<int>(EnemyAttack::Behavior::kPressAttack):
			behaviorRequest_ = Behavior::kAttack;
			enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPressAttack);
			break;
		case static_cast<int>(EnemyAttack::Behavior::kDashAttack):
			behaviorRequest_ = Behavior::kAttack;
			enemyAttack_->SetBehavior(EnemyAttack::Behavior::kDashAttack);
			break;
		case static_cast<int>(EnemyAttack::Behavior::kPunchAttack):
			behaviorRequest_ = Behavior::kAttack;
			enemyAttack_->SetBehavior(EnemyAttack::Behavior::kPunchAttack);
			break;
		case static_cast<int>(EnemyAttack::Behavior::kTornadoAttack):
			behaviorRequest_ = Behavior::kAttack;
			enemyAttack_->SetBehavior(EnemyAttack::Behavior::kTornadoAttack);
			break;
		case static_cast<int>(EnemyAttack::Behavior::kMeteoAttack):
			behaviorRequest_ = Behavior::kAttack;
			enemyAttack_->SetBehavior(EnemyAttack::Behavior::kMeteoAttack);
			break;
		}
	}
	else {
		// 移動
		Move();
		// 動き
		Motion();
	}
}
bool move = false;
void Enemy::Move() {
	vector_ = {0.0f, 0.0f, 0.0f};
	
	Vector3 end = player_->GetWorldTransform().translation_- worldTransform_.at(0).translation_;
	end.y = 0.0f;
	if (end != Vector3(0.0f, 0.0f, 0.0f)) {
		end.Normalize();
	}
	vector_ = Slerp(vector_, end, 0.01f);
	if (vector_ != Vector3(0.0f, 0.0f, 0.0f)) {
		vector_.Normalize();
	}
	velocity_ = vector_ * kMove_Speed_;
	worldTransform_.at(0).translation_ += velocity_;
	worldTransform_.at(0).translation_.y = kFloor_Distance_;
	// 角度の更新
	angle_ += 0.02f;
}

void Enemy::Motion() {
	// 全体
	Base();
	// 体
	Body();
}

void Enemy::Base() {
	EnemyRotate(vector_); 
}

void Enemy::Body() {}

void Enemy::HitBoxUpdate() {
	// AABB
	aabb_.at(0) = {
	    .center_{worldTransform_.at(0).translation_},
	    .min_{aabb_.at(0).center_ + min_},
	    .max_{aabb_.at(0).center_ + max_},
	};
	// OBB
	obb_.at(0) = {
	    .center_{worldTransform_.at(0).translation_},
	    .orientations_{
	             {1.0f, 0.0f, 0.0f},
	             {0.0f, 1.0f, 0.0f},
	             {0.0f, 0.0f, 1.0f},
	             },
	    .size_{size_}
    };
	obb_.at(0) = OBBSetRotate(obb_.at(0), worldTransform_.at(0).rotation_);
	// Sphere
	sphere_ = {
	    .center_{worldTransform_.at(0).translation_},
	    .radius_{radius_},
	};
}

void Enemy::OnCollision(const OBB& obb, uint32_t type) {
	OBB o = obb;
	uint32_t i = type;
	i;
}