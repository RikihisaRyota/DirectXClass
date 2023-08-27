#include "PlayerAttack.h"

#include "Collision.h"
#include "Draw.h"
#include "Enemy.h"
#include "EnemyHP.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "MyMath.h"
#include "Player.h"

void PlayerAttack::Initialize(std::vector<std::unique_ptr<Model>> model) {
	// 基底クラス
	BaseCharacter::Initialize(std::move(model));
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayerAttack);
	// 衝突対象を自分以外に設定
	SetCollisionMask(~kCollisionAttributePlayerAttack);
	HitBoxInitialize();
}

void PlayerAttack::Initialize() {
	if (behaviorRequest_) {
		// ふるまいを変更
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case PlayerAttack::Behavior::kRoot:
			if (ISChageAttack_Count_Start_) {
				kChageAttackCount = kChageAttackCoolTime;
				ISChageAttack_Count_Start_ = false;
			}
			if (ISTripleAttack_Count_Start_) {
				if (IsTripleAttack_) {
					kTripleAttackCount = kTripleAttack_Count;
				} else {
					kTripleAttackCount = kTripleAttackCoolTime;
				}
				ISTripleAttack_Count_Start_ = false;
			}
			hitFlag_ = false;
		default:
			break;
		case PlayerAttack::Behavior::kChargeAttack:
			if (IsChageAttack_) {
				ChageAttackInitialize();
			} else {
				player_->SetBehavior(Player::Behavior::kRoot);
				behavior_ = Behavior::kRoot;
			}
			break;
		case PlayerAttack::Behavior::kTripleAttack:
			if (IsTripleAttack_) {
				TripleAttackInitialize();
			} else {
				player_->SetBehavior(Player::Behavior::kRoot);
				behavior_ = Behavior::kRoot;
			}
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
}

void PlayerAttack::Update() {

	switch (behavior_) {
	case PlayerAttack::Behavior::kRoot:
	default:
		RootUpdate();
		break;
	case PlayerAttack::Behavior::kChargeAttack:
		ChageAttackUpdate();
		break;
	case PlayerAttack::Behavior::kTripleAttack:
		TripleAttackUpdate();
		break;
	}
	BaseCharacter::Update();
	HitBoxUpdate();
	ImGui::Begin("PlayerAttack");
	ImGui::Text("t_:%f", t_);
	ImGui::Text("ChageAttackCount:%d", kChageAttackCount);
	ImGui::Text("TripleAttackCount:%d", kTripleAttackCount);
	ImGui::Text("hitFlag:%d", hitFlag_);
	ImGui::End();
}

void PlayerAttack::Draw(const ViewProjection& viewProjection) {
	switch (behavior_) {
	case PlayerAttack::Behavior::kRoot:
	default:
		break;
	case PlayerAttack::Behavior::kChargeAttack:
		models_[static_cast<int>(Parts::WEAPON)]->Draw(
		    worldTransforms_Parts_.at(0)[static_cast<int>(Parts::WEAPON)], viewProjection);
		break;
	case PlayerAttack::Behavior::kTripleAttack:

		break;
	}
}

void PlayerAttack::ChageAttackInitialize() {
	worldTransform_.at(0) = player_->GetWorldTransform();
	worldTransforms_Parts_.at(0)[static_cast<int>(Parts::WEAPON)].rotation_.x = 0.0f;

	// 剣のデットゾーン
	slashMin_ = DegToRad(-45.0f);
	slashMax_ = DegToRad(90.0f);

	slash_Attack_Min_ = DegToRad(RadToDeg(slashMin_) + 135.0f);
	slash_Attack_Max_ = DegToRad(RadToDeg(slashMax_) + 270.0f);

	slash_Attack_Start_ = 0.0f;
	slash_ArmAngle_Start_ = DegToRad(slash_Attack_Start_ + 180.0f);
	// 攻撃の溜めモーションスピード
	charge_Speed_ = 0.01f;
	charge_T_ = 0.0f;
	// 溜めているかどうかのフラグ
	chargeFlag_ = true;
	// 降り下ろしモーション
	slash_Speed_ = 0.2f;
	slash_T_ = 0.0f;
	// 溜めてあと立てるフラグ
	slashFlag_ = false;
	// 攻撃硬直
	rigorFlag_ = false;
	rigor_Speed_ = 0.1f;
	rigor_T_ = 0.0f;

	// チャージ中は攻撃判定なし
	hitFlag_ = true;
	IsChageAttack_ = false;
}

void PlayerAttack::ChageAttackUpdate() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	worldTransform_.at(0) = player_->GetWorldTransform();
	// チャージ中
	if (chargeFlag_) {
		if (Input::GetInstance()->PushKey(DIK_Q) ||
		    (Input::GetInstance()->GetJoystickState(0, joyState) &&
		     (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y))) {
			// チャージ中は攻撃判定なし
			hitFlag_ = true;
			charge_T_ += charge_Speed_;
			worldTransforms_Parts_.at(0)[static_cast<int>(Parts::WEAPON)].rotation_.x =
			    Lerp(slash_Attack_Start_, slashMin_, Clamp(charge_T_, 0.0f, 1.0f));
			WorldTransform worldtramsform =
			    player_->GetWorldTransforms_Parts(static_cast<int>(Player::Parts::ARML));
			worldtramsform.rotation_.x =
			    Lerp(slash_ArmAngle_Start_, slash_Attack_Min_, Clamp(charge_T_, 0.0f, 1.0f));
			player_->SetWorldtransforms_Parts(
			    worldtramsform, static_cast<int>(Player::Parts::ARML));
			player_->SetWorldtransforms_Parts(
			    worldtramsform, static_cast<int>(Player::Parts::ARMR));
		} else {
			// チャージ終わり
			hitFlag_ = false;
			chargeFlag_ = false;
			slashFlag_ = true;
			// 下に移動
			// charge_T_ = 0.0f;
			slash_Attack_Start_ =
			    worldTransforms_Parts_.at(0)[static_cast<int>(Parts::WEAPON)].rotation_.x;
			slash_ArmAngle_Start_ =
			    player_->GetWorldTransforms_Parts(static_cast<int>(Player::Parts::ARML))
			        .rotation_.x;
		}
	}
	// 降り始め
	if (slashFlag_) {
		slash_T_ += slash_Speed_;
		float rotate = Lerp(slash_Attack_Start_, slashMax_, Clamp(slash_T_, 0.0f, 1.0f));
		worldTransforms_Parts_.at(0)[static_cast<int>(Parts::WEAPON)].rotation_.x = rotate;
		WorldTransform worldtramsform =
		    player_->GetWorldTransforms_Parts(static_cast<int>(Player::Parts::ARML));
		rotate = Lerp(slash_ArmAngle_Start_, slash_Attack_Max_, Clamp(slash_T_, 0.0f, 1.0f));
		worldtramsform.rotation_.x = rotate;
		player_->SetWorldtransforms_Parts(worldtramsform, static_cast<int>(Player::Parts::ARML));
		player_->SetWorldtransforms_Parts(worldtramsform, static_cast<int>(Player::Parts::ARMR));
		if (worldTransforms_Parts_.at(0)[static_cast<int>(Parts::WEAPON)].rotation_.x >=
		    slashMax_ - 0.00005f) {
			charge_T_ = 0.0f;
			slashFlag_ = false;
			slash_T_ = 0.0f;
			rigorFlag_ = true;
			ISChageAttack_Count_Start_ = true;
		}
	}
	// 攻撃硬直
	if (rigorFlag_) {
		rigor_T_ += rigor_Speed_;
		if (rigor_T_ >= 1.0f) {
			player_->SetBehaviorRequest(Player::Behavior::kRoot);
			behaviorRequest_ = Behavior::kRoot;
			Initialize();
			rigorFlag_ = false;
		}
	}
	ImGui::Begin("PlayerAttack");
	ImGui::SliderFloat3("AABB_min", &min_.x, -3.0f, 0.0f);
	ImGui::SliderFloat3("AABB_max", &max_.x, 0.0f, 6.0f);
	ImGui::SliderFloat3("OBB_size", &size_.x, 0.0f, 6.0f);
	ImGui::SliderFloat("Sphere_radius", &radius_, 0.0f, 3.0f);
	ImGui::End();
}

void PlayerAttack::TripleAttackInitialize() {
	t_ = 0.0f;
	switch (tripleAttack_Behavior_) {
	case PlayerAttack::TripleAttack::kRoot:
		FirstInitialize();
		break;
	case PlayerAttack::TripleAttack::kFirst:
		SecondInitialize();
		break;
	case PlayerAttack::TripleAttack::kSecond:
		ThirdInitialize();
		break;
	case PlayerAttack::TripleAttack::kThird:

		break;
	default:
		break;
	}
}

void PlayerAttack::RootUpdate() {
	if (!ISChageAttack_Count_Start_) {
		if (kChageAttackCount > 0) {
			kChageAttackCount--;
		} else {
			IsChageAttack_ = true;
		}
	}
	if (!ISTripleAttack_Count_Start_) {
		if (kTripleAttackCount > 0) {
			kTripleAttackCount--;
		} else {
			tripleAttack_Behavior_ = TripleAttack::kRoot;
			IsTripleAttack_ = true;
		}
	}
}

void PlayerAttack::TripleAttackUpdate() {

	switch (tripleAttack_Behavior_) {
	case PlayerAttack::TripleAttack::kFirst:
		FirstUpdate();
		break;
	case PlayerAttack::TripleAttack::kSecond:
		SecondUpdate();
		break;
	case PlayerAttack::TripleAttack::kThird:
		ThirdUpdate();
		break;
	default:
		break;
	}
}

void PlayerAttack::FirstInitialize() {
	worldTransform_.at(0) = player_->GetWorldTransform();
	first_Speed_ = 0.1f;
	armAngleStart_ = 0.0f;
	armAngleMax_ = DegToRad(90.0f);
	armSlideStart_ = 0.0f;
	armSlideMax_ = 0.5f;
	bodyAngleStart_ = 0.0f;
	bodyAngleMax_ = DegToRad(15.0f);
	tripleAttack_Behavior_ = TripleAttack::kFirst;
}

void PlayerAttack::SecondInitialize() {
	worldTransform_.at(0) = player_->GetWorldTransform();
	second_Speed_ = 0.1f;
	armAngleStart_ = 0.0f;
	armAngleMax_ = DegToRad(90.0f);
	armSlideStart_ = 0.0f;
	armSlideMax_ = 0.5f;
	bodyAngleStart_ = 0.0f;
	bodyAngleMax_ = DegToRad(15.0f);
	tripleAttack_Behavior_ = TripleAttack::kSecond;
}

void PlayerAttack::ThirdInitialize() {
	worldTransform_.at(0) = player_->GetWorldTransform();
	third_Speed_ = 0.1f;
	tripleAttack_Behavior_ = TripleAttack::kThird;
}

void PlayerAttack::FirstUpdate() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	t_ += first_Speed_;
	WorldTransform armWorldtramsform =
	    player_->GetWorldTransforms_Parts(static_cast<int>(Player::Parts::ARMR));
	WorldTransform motionWorldtramsform = player_->GetWorldTransform_Motion();

	float armrotate = Lerp(armAngleStart_, armAngleMax_, Clamp(t_, 0.0f, 1.0f));
	float translation = Lerp(armSlideStart_, armSlideMax_, Clamp(t_, 0.0f, 1.0f));
	armWorldtramsform.rotation_.x = armrotate;
	armWorldtramsform.translation_.z = translation;
	player_->SetWorldtransforms_Parts(armWorldtramsform, static_cast<int>(Player::Parts::ARMR));

	float motionbodyrotate = Lerp(bodyAngleStart_, -bodyAngleMax_, Clamp(t_, 0.0f, 1.0f));
	motionWorldtramsform.rotation_.y = motionbodyrotate;
	player_->SetWorldtransform_Motion(motionWorldtramsform);

	// 回転行列生成
	Matrix4x4 rotate = MakeRotateYMatrix(player_->GetWorldTransform().rotation_.y);
	// オフセットをカメラの回転に合わせて回転させる
	center_ = TransformNormal(center_Distance_, rotate);
	// ホーミング
	if (t_ <= 1.0f) {
		Homing();
	}
	if (t_ >= 2.0f) {
		t_ = 0.0f;
		player_->SetBehaviorRequest(Player::Behavior::kRoot);
		behaviorRequest_ = Behavior::kRoot;
		ISTripleAttack_Count_Start_ = true;
		Initialize();
	}
}

void PlayerAttack::SecondUpdate() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	t_ += second_Speed_;
	WorldTransform armWorldtramsform =
	    player_->GetWorldTransforms_Parts(static_cast<int>(Player::Parts::ARML));
	WorldTransform motionWorldtramsform = player_->GetWorldTransform_Motion();

	float armrotate = Lerp(armAngleStart_, armAngleMax_, Clamp(t_, 0.0f, 1.0f));
	float translation = Lerp(armSlideStart_, armSlideMax_, Clamp(t_, 0.0f, 1.0f));
	armWorldtramsform.rotation_.x = armrotate;
	armWorldtramsform.translation_.z = translation;
	player_->SetWorldtransforms_Parts(armWorldtramsform, static_cast<int>(Player::Parts::ARML));

	float motionbodyrotate = Lerp(bodyAngleStart_, bodyAngleMax_, Clamp(t_, 0.0f, 1.0f));
	motionWorldtramsform.rotation_.y = motionbodyrotate;
	player_->SetWorldtransform_Motion(motionWorldtramsform);

	// 回転行列生成
	Matrix4x4 rotate = MakeRotateYMatrix(player_->GetWorldTransform().rotation_.y);
	// 回転に合わせて回転させる
	center_ = TransformNormal(center_Distance_, rotate);
	// ホーミング
	if (t_ <= 1.0f) {
		Homing();
	}
	if (t_ >= 1.5f) {
		t_ = 0.0f;
		player_->SetBehaviorRequest(Player::Behavior::kRoot);
		behaviorRequest_ = Behavior::kRoot;
		ISTripleAttack_Count_Start_ = true;
		Initialize();
	}
}

void PlayerAttack::ThirdUpdate() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	t_ += third_Speed_;
	WorldTransform motionWorldtramsform = player_->GetWorldTransform_Motion();
	float bodyrotate = Lerp(0.0f, DegToRad(720.0f), Clamp(t_, 0.0f, 1.0f));
	motionWorldtramsform.rotation_.x = bodyrotate;
	player_->SetWorldtransform_Motion(motionWorldtramsform);
	// 回転行列生成
	Matrix4x4 rotate = MakeRotateYMatrix(player_->GetWorldTransform().rotation_.y);
	// 回転に合わせて回転させる
	center_ = TransformNormal(center_Distance_, rotate);
	// ホーミング
	if (t_ <= 1.0f) {
		Homing();
	}
	if (t_ >= 2.5f) {
		t_ = 0.0f;
		player_->SetBehaviorRequest(Player::Behavior::kRoot);
		behaviorRequest_ = Behavior::kRoot;
		tripleAttack_Behavior_ = TripleAttack::kRoot;
		IsTripleAttack_ = false;
		ISTripleAttack_Count_Start_ = true;
		Initialize();
	}
}

void PlayerAttack::HitBoxUpdate() {

	switch (behavior_) {
	case PlayerAttack::Behavior::kRoot:
	default:
		break;
	case PlayerAttack::Behavior::kChargeAttack:
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
		obb_.at(0) = OBBSetRotate(
		    obb_.at(0), worldTransform_.at(0).rotation_,
		    worldTransforms_Parts_.at(0)[static_cast<int>(Parts::WEAPON)].rotation_);
		break;
	case PlayerAttack::Behavior::kTripleAttack:
		// AABB
		 aabb_.at(0) = {
		     .center_{worldTransform_.at(0).translation_},
		     .min_{aabb_.at(0).center_ + tripleAttackMin_},
		     .max_{aabb_.at(0).center_ + tripleAttackMax_},
		 };
		// OBB
		 obb_.at(0) = {
		     .center_{worldTransform_.at(0).translation_ + center_},
		     .orientations_{
		              {1.0f, 0.0f, 0.0f},
		              {0.0f, 1.0f, 0.0f},
		              {0.0f, 0.0f, 1.0f},
		              },
		     .size_{tripleAttackSize_}
		      };
		 obb_.at(0) = OBBSetRotate(obb_.at(0), worldTransform_.at(0).rotation_);

		break;
	}
}

void PlayerAttack::Homing() {
	// 範囲内にはいていたらホーミング
	if (IsCollision(*enemy_->GetAABB(), aabb_.at(0))) {
		Vector3 toEnemy =
		    enemy_->GetWorldTransform().translation_ - worldTransform_.at(0).translation_;
		// 長さが1.0f以上ならホーミング
		const float kLength = player_->GetOBB()->size_.x + enemy_->GetOBB()->size_.x;
		float distance = toEnemy.Length();
		if (distance >= kLength) {
			toEnemy.Normalize();
			worldTransform_.at(0).translation_ += Lerp(Vector3(0.0f, 0.0f, 0.0f), toEnemy, 0.4f);
			worldTransform_.at(0).translation_.y = 1.0f;
			player_->SetTranslation(worldTransform_.at(0).translation_);
			player_->PlayerRotate(toEnemy);
		} else {
			// それ以下ならプレイヤーの回転だけセットする
			toEnemy.Normalize();
			player_->PlayerRotate(toEnemy);
		}
	}
}

void PlayerAttack::HitBoxInitialize() {
	// AABB
	min_ = {-5.1f, -0.9f, -5.1f};
	max_ = {5.1f, 5.1f, 5.1f};
	tripleAttackMin_ = {-4.0f, -0.9f, -4.0f};
	tripleAttackMax_ = {4.0f, 1.0f, 4.0f};
	// OBB
	size_ = {1.0f, 5.1f, 0.5f};
	tripleAttackSize_ = {1.0f, 1.0f, 1.0f};

	center_Distance_ = {0.0f, 0.0f, 1.0f};
	// Sphere
	radius_ = 1.2f;
	// AABB
	AABB aabb;
	aabb_.emplace_back(aabb);
	// OBB
	OBB obb;
	obb_.emplace_back(obb);
}
void PlayerAttack::HitBoxDraw(const ViewProjection& viewProjection) {
	DrawAABB(aabb_.at(0), viewProjection, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	DrawOBB(obb_.at(0), viewProjection, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void PlayerAttack::OnCollision(const OBB& obb, uint32_t type) {
	OBB a = obb;
	uint32_t i = type;
	i;
	switch (behavior_) {
	case PlayerAttack::Behavior::kRoot:
	default:
		break;
	case PlayerAttack::Behavior::kChargeAttack:
		if (!hitFlag_) {
			EnemyHP::SetAdd(static_cast<uint32_t>(30 * (charge_T_ + 1.0f)));
			hitFlag_ = true;
		}

		break;
	case PlayerAttack::Behavior::kTripleAttack:

		switch (tripleAttack_Behavior_) {
		case PlayerAttack::TripleAttack::kFirst:
			if (!hitFlag_) {
				EnemyHP::SetAdd(5);
				hitFlag_ = true;
			}
			break;
		case PlayerAttack::TripleAttack::kSecond:
			if (!hitFlag_) {
				EnemyHP::SetAdd(10);
				hitFlag_ = true;
			}
			break;
		case PlayerAttack::TripleAttack::kThird:
			if (!hitFlag_) {
				EnemyHP::SetAdd(15);
				hitFlag_ = true;
			}
			break;
		default:
			break;
		}
		break;
	}
}
