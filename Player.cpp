#include "Player.h"

#include <cassert>

#include "Draw.h"
#include "GlobalVarriables.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "TextureManager.h"

void Player::Initialize(std::vector<std::unique_ptr<Model>> model) {
	// 基底クラス
	BaseCharacter::Initialize(std::move(model));
	//SetGlobalVariables();
	//GetGlobalVariables();
	worldTransform_.at(0).translation_.y = 10.0f;
	BaseCharacter::Update();
	destinationAngle_ = { 0.0f, 0.0f, 1.0f };
	// 転送
	BaseCharacter::Update();
	// AABB
	aabb_.resize(1);
	obb_.resize(1);
	// 方向
	interRotate_ = { 0.0f, 0.0f, 1.0f };
	// ジャンプフラグ
	isJump = false;
	// ダッシュフラグ
	IsDash_ = false;
	// 浮遊アニメーションの初期化
	InitializeFloatGimmick();
#pragma region 当たり判定
	HitBoxInitialize(kCollisionAttributePlayer);
#pragma endregion
}

void Player::Update() {
	if (behaviorRequest_) {
		// ふるまいを変更
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Player::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Player::Behavior::kDash:
			BehaviorDashInitialize();
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_) {
	case Player::Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kDash:
		BehaviorDashUpdate();
		break;
	}
	//ChackTranslation();
	HitBoxUpdate();

	// 転送
	BaseCharacter::Update();
}

void Player::BehaviorRootUpdate() {
	// 入力
	GamePadInput();
	// プレイヤーモーションの更新
	Motion();
	// ダッシュのクールタイム
	if (dash_Count_Start_Flag_) {
		dash_Count_--;
		if (dash_Count_ <= 0) {
			dash_Count_Start_Flag_ = false;
			IsDash_ = false;
		}
	}
}

void Player::BehaviorDashInitialize() {
	workDash_.dashParameter_ = 0;
	worldTransform_.at(0).rotation_.y = std::atan2(destinationAngle_.x, destinationAngle_.z);
	acceleration_.y = 0.0f;
	IsDash_ = true;
	dash_Count_Start_Flag_ = true;
	dash_Count_ = kDash_CoolTime_;
}

void Player::BehaviorDashUpdate() {

	// 自キャラの向いている方向に移動する処理
	const float kDashSpeed = 0.5f;
	velocity_ = destinationAngle_ * kDashSpeed;
	worldTransform_.at(0).translation_ += velocity_;

	// 基底の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= kDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}

	worldTransform_Motion_.at(0).rotation_.x = Lerp(
		DegToRad(0.0f), DegToRad(360.0f),
		static_cast<float>(workDash_.dashParameter_) / static_cast<float>(kDashTime));
	if (workDash_.dashParameter_ < kDashTime * 0.5f) {
		worldTransform_Motion_.at(0).translation_.y = Lerp(
			0, 0.5f, static_cast<float>(workDash_.dashParameter_) / static_cast<float>(kDashTime));
	}
	else {
		worldTransform_Motion_.at(0).translation_.y = Lerp(
			0.5f, 0.0f,
			static_cast<float>(workDash_.dashParameter_) / static_cast<float>(kDashTime));
	}
}

void Player::HitBoxUpdate() {
	// AABB
	aabb_.at(0) = {
		.center_{worldTransform_.at(0).translation_},
		.min_{worldTransform_.at(0).translation_ + min_},
		.max_{worldTransform_.at(0).translation_ + max_},
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

void Player::SetGlobalVariables() {
	GlobalVariables* globalVariables = nullptr;
	globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループ追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	GlobalVariables::GetInstance()->SaveFile(groupName);
}

void Player::GetGlobalVariables() {
	GlobalVariables* globalVariables = nullptr;
	globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	worldTransform_.at(0).translation_ = globalVariables->GetValue<Vector3>(groupName, "position");
	// 転送
	BaseCharacter::Update();
}

void Player::OnCollision(const OBB& obb, uint32_t type) {
	switch (type) {
	case static_cast<uint32_t>(Collider::Type::PlayerToEnemy):
	{
		// OBB同士が衝突していると仮定して、重なり領域を計算する
		// ここでは、OBB同士の各軸方向での重なりの幅を計算し、最小値を取得する
		Vector3 distance = obb.center_ - obb_.at(0).center_;

		// 当たり判定が成功したので押し戻し処理を行う
		float overlapX = obb_.at(0).size_.x + obb.size_.x - std::abs(distance.x);
		float overlapY = obb_.at(0).size_.y + obb.size_.y - std::abs(distance.y);
		float overlapZ = obb_.at(0).size_.z + obb.size_.z - std::abs(distance.z);

		if (overlapX < overlapY && overlapX < overlapZ) {
			if (distance.x < 0.0f) {
				obb_.at(0).center_ += Vector3{ overlapX, 0, 0 };
			}
			else {
				obb_.at(0).center_ += Vector3{ -overlapX, 0, 0 };
			}
		}
		else if (overlapY < overlapX && overlapY < overlapZ) {
			if (distance.y < 0.0f) {
				obb_.at(0).center_ += Vector3{ 0, overlapY, 0 };
			}
			else {
				obb_.at(0).center_ += Vector3{ 0, -overlapY, 0 };
			}
		}
		else {
			if (distance.z < 0.0f) {
				obb_.at(0).center_ += Vector3{ 0, 0, overlapZ };
			}
			else {
				obb_.at(0).center_ += Vector3{ 0, 0, -overlapZ };
			}
		}
		worldTransform_.at(0).translation_ = obb_.at(0).center_;
		// 転送
		BaseCharacter::Update();
		HitBoxUpdate();
		break;
	}
	case static_cast<uint32_t>(Collider::Type::PlayerToBlock):
	{
		// OBB同士が衝突していると仮定して、重なり領域を計算する-0.399999619
		// ここでは、OBB同士の各軸方向での重なりの幅を計算し、最小値を取得する
		Vector3 distance = obb.center_ - obb_.at(0).center_;

		// 当たり判定が成功したので押し戻し処理を行う
		float overlapX = obb_.at(0).size_.x + obb.size_.x - std::abs(distance.x);
		float overlapY = obb_.at(0).size_.y + obb.size_.y - std::abs(distance.y);
		float overlapZ = obb_.at(0).size_.z + obb.size_.z - std::abs(distance.z);

		if (overlapX < overlapY && overlapX < overlapZ) {
			if (distance.x < 0.0f) {
				obb_.at(0).center_ += Vector3{ static_cast<float>(overlapX + 0.1f), 0, 0 };
			}
			else {
				obb_.at(0).center_ += Vector3{ static_cast<float>(-overlapX - 0.1f), 0, 0 };
			}
		}
		else if (overlapY < overlapX && overlapY < overlapZ) {
			if (distance.y < 0.0f) {
				obb_.at(0).center_ += Vector3{ 0,static_cast<float>(overlapY) , 0 };
			}
			else {
				obb_.at(0).center_ += Vector3{ 0, static_cast<float>(-overlapY), 0 };
			}
			acceleration_.y = 0.0f;
			isJump = false;
		}
		else {
			if (distance.z < 0.0f) {
				obb_.at(0).center_ += Vector3{ 0, 0, static_cast<float>(overlapZ + 0.1f) };
			}
			else {
				obb_.at(0).center_ += Vector3{ 0, 0, static_cast<float>(-overlapZ - 0.1f)};
			}
		}
		worldTransform_.at(0).translation_ = obb_.at(0).center_;
		
		// 転送
		BaseCharacter::Update();
		HitBoxUpdate();
		break;
	}
	}

}

void Player::Draw(const ViewProjection& viewProjection) {
	models_[static_cast<int>(Parts::HEAD)]->Draw(
		worldTransforms_Parts_.at(0)[static_cast<int>(Parts::HEAD)], viewProjection);
	models_[static_cast<int>(Parts::BODY)]->Draw(
		worldTransforms_Parts_.at(0)[static_cast<int>(Parts::BODY)], viewProjection);
	models_[static_cast<int>(Parts::ARML)]->Draw(
		worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARML)], viewProjection);
	models_[static_cast<int>(Parts::ARMR)]->Draw(
		worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARMR)], viewProjection);
}

void Player::HitBoxInitialize(uint32_t collisionMask) {
	// 衝突属性を設定
	SetCollisionAttribute(collisionMask);
	// 衝突対象を自分以外に設定
	SetCollisionMask(~collisionMask);
	// AABB
	min_ = { -1.0f, -1.0f, -1.0f };
	max_ = { 1.0f, 1.0f, 1.0f };
	// OBB
	size_ = { 0.5f, 1.0f, 0.5f };
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
		.center_{
				 worldTransform_.at(0).translation_.x, worldTransform_.at(0).translation_.y + 3.0f,
				 worldTransform_.at(0).translation_.z},
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

void Player::HitBoxDraw(const ViewProjection& viewProjection) {
	DrawAABB(aabb_.at(0), viewProjection, Vector4(0.0f, 0.5f, 0.25f, 1.0f));
	DrawOBB(obb_.at(0), viewProjection, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void Player::PlayerRotate(const Vector3& vector1) {
	Vector3 vector = vector1;
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

void Player::BehaviorInitialize() {
	if (behaviorRequest_) {
		// ふるまいを変更
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Player::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Player::Behavior::kDash:
			BehaviorDashInitialize();
			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
}

void Player::GamePadInput() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	// プレイヤー移動
	Move();
	// 攻撃開始
	/*if (Input::GetInstance()->TriggerKey(DIK_Q) ||
		(Input::GetInstance()->GetJoystickState(0, joyState) &&
			(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y))) {
		behaviorRequest_ = Behavior::kAttack;
		playerAttack_->SetBehavior(PlayerAttack::Behavior::kChargeAttack);
	}
	if (Input::GetInstance()->TriggerKey(DIK_E) ||
		(Input::GetInstance()->GetJoystickState(0, joyState) &&
			(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
		behaviorRequest_ = Behavior::kAttack;
		playerAttack_->SetBehavior(PlayerAttack::Behavior::kTripleAttack);
	}*/
	// ダッシュ開始
	if ((!IsDash_) &&
		(Input::GetInstance()->TriggerKey(DIK_LSHIFT) /*||
			(Input::GetInstance()->GetJoystickState(0, joyState) &&
				(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X))*/)) {
		behaviorRequest_ = Behavior::kDash;
	}
	// ジャンプ
	Jump();
	// 重力
	Gravity();
}

void Player::Move() {
	// 移動量
	vector_ = { 0.0f, 0.0f, 0.0f };
#pragma region ゲームパット
	//// ゲームパットの状態を得る変数
	//XINPUT_STATE joyState{};
	//// ゲームパットの状況取得
	//// 入力がなかったら何もしない
	//if (Input::GetInstance()->GetJoystickState(0, joyState)) {
	//	const float kMargin = 0.7f;
	//	// 移動量
	//	Vector3 move = {
	//		static_cast<float>(joyState.Gamepad.sThumbLX),
	//		0.0f,
	//		static_cast<float>(joyState.Gamepad.sThumbLY),
	//	};
	//	if (move.Length() > kMargin) {
	//		vector_ = {
	//			static_cast<float>(joyState.Gamepad.sThumbLX),
	//			0.0f,
	//			static_cast<float>(joyState.Gamepad.sThumbLY),
	//		};
	//	}
	//}
#pragma endregion
#pragma region キーボード
	if (Input::GetInstance()->PushKey(DIK_W)) {
		vector_.z = 1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		vector_.z = -1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		vector_.x = -1.0f;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		vector_.x = 1.0f;
	}
	// 移動量に速さを反映
	if (vector_ != Vector3(0.0f, 0.0f, 0.0f)) {
		vector_.Normalize();
	}
#pragma endregion
#pragma region 正規化と回転
	// 移動量に速さを反映
	if (vector_ != Vector3(0.0f, 0.0f, 0.0f)) {
		vector_.Normalize();
		// 回転行列生成
		Matrix4x4 rotate = MakeRotateYMatrix(viewProjection_->rotation_.y);
		// オフセットをカメラの回転に合わせて回転させる
		vector_ = TransformNormal(vector_, rotate);
		destinationAngle_ = vector_;
	}
#pragma endregion
}

void Player::Jump() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	// ゲームパットの状況取得
	// 入力がなかったら何もしない
	if (((Input::GetInstance()->GetJoystickState(0, joyState) &&
		(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) ||
		Input::GetInstance()->TriggerKey(DIK_SPACE)) &&
		!isJump) {
		acceleration_.y = kJumpPower;
		isJump = true;
	}
}
void Player::Gravity() {
	velocity_ = vector_ * kSpeed;
	if (acceleration_.y >= -0.5f) {
		acceleration_.y -= kGravity;
	}
	velocity_+= acceleration_;
	worldTransform_.at(0).translation_ += velocity_;

	if (std::fabs(velocity_.x) <= 0.001 && std::fabs(velocity_.z) <= 0.001) {
		velocity_.x = 0.0f;
		velocity_.z = 0.0f;
	}

}
void Player::ChackTranslation() {
	/*if (worldTransform_.at(0).translation_.x - worldTransform_.at(0).scale_.x <=
		-ground_->GetEdge()) {
		worldTransform_.at(0).translation_.x = -ground_->GetEdge() + worldTransform_.at(0).scale_.x;
	}
	else if (
		worldTransform_.at(0).translation_.x + worldTransform_.at(0).scale_.x >=
		ground_->GetEdge()) {
		worldTransform_.at(0).translation_.x = ground_->GetEdge() - worldTransform_.at(0).scale_.x;
	}
	if (worldTransform_.at(0).translation_.z - worldTransform_.at(0).scale_.z <=
		-ground_->GetEdge()) {
		worldTransform_.at(0).translation_.z = -ground_->GetEdge() + worldTransform_.at(0).scale_.z;
	}
	else if (
		worldTransform_.at(0).translation_.z + worldTransform_.at(0).scale_.z >=
		ground_->GetEdge()) {
		worldTransform_.at(0).translation_.z = ground_->GetEdge() - worldTransform_.at(0).scale_.z;
	}*/

}
void Player::PlayerRotate() {
	if (vector_ != Vector3(0.0f, 0.0f, 0.0f)) {
		vector_.Normalize();
	}
	if (interRotate_ != Vector3(0.0f, 0.0f, 0.0f)) {
		interRotate_.Normalize();
	}
	Vector3 rotate = Lerp(interRotate_, vector_, kTurn);
	//  Y軸回り角度(θy)
	worldTransform_.at(0).rotation_.y = std::atan2(rotate.x, rotate.z);
	// プレイヤーの向いている方向
	interRotate_ = rotate;
}
void Player::InitializeFloatGimmick() {
	floatingParameter_ = 0.0f;
	worldTransform_Motion_.at(0).rotation_ = { 0.0f, 0.0f, 0.0f };
	worldTransform_Motion_.at(0).translation_ = { 0.0f, 0.0f, 0.0f };
	worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARMR)].translation_ = Vector3(0.0f, 0.0f, 0.0f);
	worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARMR)].rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARML)].translation_ = Vector3(0.0f, 0.0f, 0.0f);
	worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARML)].rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	BaseCharacter::Update();
}
void Player::Motion() {
	// 全体
	Base();
	// 頭
	Head();
	// 体
	Body();
	// 左腕
	ArmLeft();
	// 右腕
	ArmRight();
	// 転送
	// UpdateMotionMatrix();
}
void Player::UpdateMotionMatrix() {}
void Player::UpdateFloatGimmick() {
	// 1フレームでのパラメータ加算値
	const float kFroatStep =
		2.0f * static_cast<float>(3.14159265359) / static_cast<float>(kFloatAnimationCycle);
	// パラメータを1ステップ分加算
	floatingParameter_ += kFroatStep;
	// 2πを超えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * static_cast<float>(3.14159265359));
	// 浮遊を座標に反映
	worldTransform_Motion_.at(0).translation_.y = (std::sin(floatingParameter_) * kFloatAmplitude);
}
void Player::Base() {
	// プレイヤーの回転イージング
	PlayerRotate();
	// 浮いてるモーション
	UpdateFloatGimmick();
}
void Player::ArmLeft() {
	// 浮遊を座標に反映
	worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARML)].rotation_.x =
		std::sin(floatingParameter_) * kArmRAmplitude;
}
void Player::ArmRight() { // 1フレームでのパラメータ加算値
	// 浮遊を座標に反映
	worldTransforms_Parts_.at(0)[static_cast<int>(Parts::ARMR)].rotation_.x =
		std::cos(floatingParameter_) * kArmLAmplitude;
}
void Player::Head() {}
void Player::Body() {}
void Player::BehaviorRootInitialize() { InitializeFloatGimmick(); }