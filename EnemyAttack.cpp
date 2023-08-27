#include "EnemyAttack.h"

#include "Collision.h"
#include "Draw.h"
#include "Enemy.h"
#include "Input.h"
#include "MyMath.h"
#include "Player.h"
#include "PlayerHP.h"

void EnemyAttack::Initialize(std::vector<std::unique_ptr<Model>> model) {
	// 基底クラス
	BaseCharacter::Initialize(std::move(model));
	worldTransform_.at(0).scale_ = {4.0f, 1.0f, 4.0f};
	worldTransform_.at(0).UpdateMatrix();
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemyAttack);
	// 衝突対象を自分以外に設定
	SetCollisionMask(~kCollisionAttributeEnemyAttack);
	HitBoxInitialize();

	press_ = std::make_unique<EnemyPress>();
	press_->SetPlayerEnemy(player_, enemy_, this);

	dash_ = std::make_unique<EnemyDash>();
	dash_->SetPlayerEnemy(player_, enemy_, this);

	punch_ = std::make_unique<EnemyPunch>();
	punch_->SetPlayerEnemy(player_, enemy_, this);

	tornade_ = std::make_unique<EnemyTornado>();
	tornade_->SetPlayerEnemy(player_, enemy_, this);

	meteo_ = std::make_unique<EnemyMeteo>();
	meteo_->SetPlayerEnemy(player_, enemy_, this);
}

void EnemyAttack::Initialize() {
	if (behaviorRequest_) {
		// ふるまいを変更
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case EnemyAttack::Behavior::kRoot:
		default:
			RootInitialize();
			break;
		case EnemyAttack::Behavior::kPressAttack:
			press_->Initialize();
			break;
		case EnemyAttack::Behavior::kDashAttack:
			dash_->Initialize();
			break;
		case EnemyAttack::Behavior::kPunchAttack:
			punch_->Initialize();
			break;
		case EnemyAttack::Behavior::kTornadoAttack:
			tornade_->Initialize();
			break;
		case EnemyAttack::Behavior::kMeteoAttack:
			meteo_->Initialize();
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
		// ヒットボックスの再設定
		HitBoxInitialize();
	}
}

void EnemyAttack::RootInitialize() {
	BaseCharacter::ResetCount();
	worldTransform_.at(0).scale_ = {4.0f, 1.0f, 4.0f};
	worldTransform_.at(0).rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.at(0).translation_ = {0.0f, 0.0f, 0.0f};
	BaseCharacter::Update();
}

void EnemyAttack::Update() {
	if (enemy_->GetBehavior() == Enemy::Behavior::kAttack) {
		if (behaviorRequest_) {
			// ふるまいを変更
			behavior_ = behaviorRequest_.value();
			// 各ふるまいごとの初期化を実行
			switch (behavior_) {
			case EnemyAttack::Behavior::kRoot:
			default:
				RootInitialize();
				break;
			case EnemyAttack::Behavior::kPressAttack:
				press_->Initialize();
				break;
			case EnemyAttack::Behavior::kDashAttack:
				dash_->Initialize();
				break;
			case EnemyAttack::Behavior::kPunchAttack:
				punch_->Initialize();
				break;
			case EnemyAttack::Behavior::kTornadoAttack:
				tornade_->Initialize();
				break;
			case EnemyAttack::Behavior::kMeteoAttack:
				meteo_->Initialize();
				break;
			}
			// ふるまいリクエストをリセット
			behaviorRequest_ = std::nullopt;
		}
		switch (behavior_) {
		case EnemyAttack::Behavior::kRoot:
		default:
			enemy_->SetBehavior(Enemy::Behavior::kRoot);
			break;
		case EnemyAttack::Behavior::kPressAttack:
			press_->Update();
			if (!press_->GetWorking()) {
				behaviorRequest_ = EnemyAttack::Behavior::kRoot;
			}
			break;
		case EnemyAttack::Behavior::kDashAttack:
			dash_->Update();
			if (!dash_->GetWorking()) {
				behaviorRequest_ = EnemyAttack::Behavior::kRoot;
			}
			break;
		case EnemyAttack::Behavior::kPunchAttack:
			punch_->Update();
			if (!punch_->GetWorking()) {
				behaviorRequest_ = EnemyAttack::Behavior::kRoot;
			}
			break;
		case EnemyAttack::Behavior::kTornadoAttack:
			tornade_->Update();
			if (!tornade_->GetWorking()) {
				behaviorRequest_ = EnemyAttack::Behavior::kRoot;
			}
			break;
		case EnemyAttack::Behavior::kMeteoAttack:
			meteo_->Update();
			if (!meteo_->GetWorking()) {
				behaviorRequest_ = EnemyAttack::Behavior::kRoot;
			}
			break;
		}
		BaseCharacter::Update();
		HitBoxUpdate();
	}
}

void EnemyAttack::Draw(const ViewProjection& viewProjection) {
	for (size_t i = 0; i < worldTransform_.size(); i++) {
		switch (behavior_) {
		case EnemyAttack::Behavior::kRoot:
		default:
			break;
		case EnemyAttack::Behavior::kPressAttack:
			models_[static_cast<int>(EnemyAttack::Parts::CIRCLE)]->Draw(
			    worldTransforms_Parts_.at(i)[static_cast<int>(EnemyAttack::Parts::CIRCLE)],
			    viewProjection);
			break;
		case EnemyAttack::Behavior::kDashAttack:
			models_[static_cast<int>(EnemyAttack::Parts::PLANE)]->Draw(
			    worldTransforms_Parts_.at(i)[static_cast<int>(EnemyAttack::Parts::PLANE)],
			    viewProjection);
			break;
		case EnemyAttack::Behavior::kPunchAttack:
			models_[static_cast<int>(EnemyAttack::Parts::CIRCLE)]->Draw(
			    worldTransforms_Parts_.at(i)[static_cast<int>(EnemyAttack::Parts::CIRCLE)],
			    viewProjection);
		case EnemyAttack::Behavior::kTornadoAttack:
			models_[static_cast<int>(EnemyAttack::Parts::CIRCLE)]->Draw(
			    worldTransforms_Parts_.at(i)[static_cast<int>(EnemyAttack::Parts::CIRCLE)],
			    viewProjection);
			break;
		case EnemyAttack::Behavior::kMeteoAttack:
			models_[static_cast<int>(EnemyAttack::Parts::CIRCLE)]->Draw(
			    worldTransforms_Parts_.at(i)[static_cast<int>(EnemyAttack::Parts::CIRCLE)],
			    viewProjection);
			models_[static_cast<int>(EnemyAttack::Parts::METEO)]->Draw(
			    worldTransforms_Parts_.at(i)[static_cast<int>(EnemyAttack::Parts::METEO)],
			    viewProjection);
			break;
		}
	}
}

void EnemyAttack::HitBoxInitialize() {
	aabb_.clear();
	obb_.clear();
	size_t size = worldTransform_.size();
	aabb_.resize(size);
	obb_.resize(size);

	for (size_t i = 0; i < size; i++) {
		// AABB
		min_ = {-5.0f, -0.9f, -5.0f};
		max_ = {5.0f, 1.0f, 5.0f};
		// OBB
		size_ = {4.0f, 1.0f, 4.0f};
		// Sphere
		radius_ = 1.2f;
		// AABB
		aabb_.at(i) = {
		    .center_{worldTransform_.at(i).translation_},
		    .min_{aabb_.at(i).center_ - worldTransform_.at(i).scale_},
		    .max_{aabb_.at(i).center_ + worldTransform_.at(i).scale_},
		};
		// OBB
		obb_.at(i) = {
		    .center_{worldTransform_.at(i).translation_},
		    .orientations_{
		             {1.0f, 0.0f, 0.0f},
		             {0.0f, 1.0f, 0.0f},
		             {0.0f, 0.0f, 1.0f},
		             },
		    .size_{worldTransform_.at(i).scale_}
        };
		obb_.at(i) = OBBSetRotate(obb_.at(i), worldTransform_.at(i).rotation_);
		// Sphere
		sphere_ = {
		    .center_{worldTransform_.at(i).translation_},
		    .radius_{radius_},
		};
	}
}

void EnemyAttack::HitBoxUpdate() {
	aabb_.clear();
	obb_.clear();
	
	for (size_t i = 0; i < worldTransform_.size(); i++) {
		float size = (std::max)(
		    (std::max)(worldTransform_.at(i).scale_.x, worldTransform_.at(i).scale_.y),
		    worldTransform_.at(i).scale_.z);
		// AABB
		AABB aabb;
		aabb = {
		    .center_{worldTransform_.at(i).translation_},
		    .min_{aabb.center_ - size},
		    .max_{aabb.center_ + size},
		};
		aabb.center_ = worldTransform_.at(i).translation_;
		aabb.min_ = {aabb.center_ - size};
		aabb.max_ = {aabb.center_ + size};
		aabb_.emplace_back(aabb);
		// OBB
		OBB obb;
		obb = {
		    .center_{worldTransform_.at(i).translation_},
		    .orientations_{
		             {1.0f, 0.0f, 0.0f},
		             {0.0f, 1.0f, 0.0f},
		             {0.0f, 0.0f, 1.0f},
		             },
		    .size_{worldTransform_.at(i).scale_}
        };
		obb = OBBSetRotate(obb, worldTransform_.at(i).rotation_);
		obb_.emplace_back(obb);
	}
}

void EnemyAttack::HitBoxDraw(const ViewProjection& viewProjection) {
	for (size_t i = 0; i < aabb_.size(); i++) {
		DrawAABB(aabb_.at(i), viewProjection, Vector4(0.0f, 0.5f, 0.25f, 1.0f));
		DrawOBB(obb_.at(i), viewProjection, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

void EnemyAttack::OnCollision(const OBB& obb, uint32_t type) {
	OBB obb1 = obb;
	uint32_t t = type;
	t;
	switch (behavior_) {
	case EnemyAttack::Behavior::kRoot:
	default:
		break;
	case EnemyAttack::Behavior::kPressAttack:
		if (press_->GetAttack()) {
			for (size_t i = 0; i < worldTransform_.size(); i++) {
				if (IsCollision(
				        OBB(*player_->GetOBB(0)), Sphere(GetOBB(i)->center_, GetOBB(i)->size_.z))) {
					PlayerHP::SetAdd(70);
					press_->SetHit(true);
				}
			}
		}
		break;
	case EnemyAttack::Behavior::kDashAttack:
		if (dash_->GetAttack()) {
			for (size_t i = 0; i < worldTransform_.size(); i++) {
				if (IsCollision(*player_->GetOBB(0), *enemy_->GetOBB(i))) {
					PlayerHP::SetAdd(40);
					dash_->SetHit(true);
				}
			}
		}
		break;
	case EnemyAttack::Behavior::kPunchAttack:
		if (punch_->GetAttack()) {
			PlayerHP::SetAdd(30);
			punch_->SetHit(true);
		}
		break;
	case EnemyAttack::Behavior::kTornadoAttack:
		if (tornade_->GetAttack()) {
			// 連続ヒット
			PlayerHP::SetAdd(7);
			// tornade_->SetHit(true);
		}
		break;
	case EnemyAttack::Behavior::kMeteoAttack:
		if (meteo_->GetAttack()) {
		PlayerHP::SetAdd(5);
		//meteo_->SetHit(true);
		}
	break;
	}
}