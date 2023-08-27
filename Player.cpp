#include "Player.h"

#include <cassert>

#include "Draw.h"
#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "PlayerAttack.h"
#include "TextureManager.h"

// テスト
#include "PrimitiveDrawer.h"

void Player::SetSpritePos() {
	chage_Position_ = {test_pos_.x, test_pos_.y - distance_Y};
	triple_Position_ = {test_pos_.x + distance_X, test_pos_.y + distance_Y};
	dash_Position_ = {test_pos_.x - distance_X, test_pos_.y + distance_Y};

	y_Position_ = {test_buttan_pos_.x, test_buttan_pos_.y - distance_buttan_Y};
	b_Position_ = {test_buttan_pos_.x + distance_buttan_X, test_buttan_pos_.y + distance_buttan_Y};
	x_Position_ = {test_buttan_pos_.x - distance_buttan_X, test_buttan_pos_.y + distance_buttan_Y};

	chage_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	triple_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	dash_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	
	/*triple_Back_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	chage_Back_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	dash_Back_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));*/

	chage_Sprite_->SetPosition(chage_Position_);
	triple_Sprite_->SetPosition(triple_Position_);
	dash_Sprite_->SetPosition(dash_Position_);

	/*chage_Back_Sprite_->SetPosition(Vector2(chage_Position_.x, chage_Position_.y + test_Scale_));
	triple_Back_Sprite_->SetPosition(Vector2(triple_Position_.x, triple_Position_.y + test_Scale_));
	dash_Back_Sprite_->SetPosition(Vector2(dash_Position_.x, dash_Position_.y + test_Scale_));*/
	
	x_Buttan_Sprite_->SetSize(Vector2(test_buttan_Scale_, test_buttan_Scale_));
	b_Buttan_Sprite_->SetSize(Vector2(test_buttan_Scale_, test_buttan_Scale_));
	y_Buttan_Sprite_->SetSize(Vector2(test_buttan_Scale_, test_buttan_Scale_));

	x_Buttan_Sprite_->SetPosition(x_Position_);
	b_Buttan_Sprite_->SetPosition(b_Position_);
	y_Buttan_Sprite_->SetPosition(y_Position_);
}

void Player::Initialize(std::vector<std::unique_ptr<Model>> model) {
	// 基底クラス
	BaseCharacter::Initialize(std::move(model));
	// SetGlobalVariables();
	GetGlobalVariables();
	worldTransform_.at(0).translation_.y = kGroundDistanse;
	BaseCharacter::Update();
	destinationAngle_ = {0.0f, 0.0f, 1.0f};
	// 転送
	BaseCharacter::Update();
	// AABB
	aabb_.resize(1);
	obb_.resize(1);
	// 方向
	interRotate_ = {0.0f, 0.0f, 1.0f};
	// ジャンプフラグ
	isJump = false;
	// ダッシュフラグ
	IsDash_ = false;
	// 浮遊アニメーションの初期化
	InitializeFloatGimmick();
#pragma region 当たり判定
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分以外に設定
	SetCollisionMask(~kCollisionAttributePlayer);

	HitBoxInitialize();
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
		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
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
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Player::Behavior::kDash:
		BehaviorDashUpdate();
		break;
	}
	SpriteUpdate();
	ChackTranslation();
	HitBoxUpdate();

	// 転送
	BaseCharacter::Update();

	// #ifdef DEBUG
	ImGui::Begin("Player");
	ImGui::SliderFloat("pos_X", &test_pos_.x, 0.0f, 1280.0f);
	ImGui::SliderFloat("pos_Y", &test_pos_.y, 0.0f, 720.0f);
	ImGui::SliderFloat("distance_X", &distance_X, 0.0f, 200.0f);
	ImGui::SliderFloat("distance_Y", &distance_Y, 0.0f, 200.0f);
	ImGui::SliderFloat("scale", &test_Scale_, 0.0f, 300.0f);
	

	/*ImGui::Text(
	    "translation_ x:%f,y:%f,z:%f", worldTransform_.translation_.x,
	    worldTransform_.translation_.y, worldTransform_.translation_.z);
	ImGui::Text(
	    "rotate_ x:%f,y:%f,z:%f", worldTransform_.rotation_.x, worldTransform_.rotation_.y,
	    worldTransform_.rotation_.z);
	ImGui::Text("interRotate_:%f,y:%f,z:%f", interRotate_.x, interRotate_.y, interRotate_.z);
	ImGui::Text(
	    "destinationAngle_:%f,y:%f,z:%f", destinationAngle_.x, destinationAngle_.y,
	    destinationAngle_.z);
	ImGui::Text("vector_x:%f,y:%f,z:%f", vector_.x, vector_.y, vector_.z);
	ImGui::Text("velocity_:%f,y:%f,z:%f", velocity_.x, velocity_.y, velocity_.z);
	ImGui::Text("acceleration_:%f,y:%f,z:%f", acceleration_.x, acceleration_.y, acceleration_.z);

	ImGui::SliderFloat3("AABB_min", &min_.x, -3.0f, 0.0f);
	ImGui::SliderFloat3("AABB_max", &max_.x, 0.0f, 3.0f);
	ImGui::SliderFloat("Sphere_radius", &radius_, 0.0f, 3.0f);*/
	ImGui::End();
	ImGui::Begin("Buttan");
	ImGui::SliderFloat("buttan_pos_X", &test_buttan_pos_.x, 0.0f, 1280.0f);
	ImGui::SliderFloat("buttan_pos_Y", &test_buttan_pos_.y, 0.0f, 720.0f);
	ImGui::SliderFloat("buttan_distance_X", &distance_buttan_X, 0.0f, 200.0f);
	ImGui::SliderFloat("buttan_distance_Y", &distance_buttan_Y, 0.0f, 200.0f);
	ImGui::SliderFloat("buttan_scale", &test_buttan_Scale_, 0.0f, 300.0f);
	ImGui::End();
	// #endif // DEBUG
	SetSpritePos();
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

void Player::BehaviorAttackInitialize() { playerAttack_->Initialize(); }

void Player::BehaviorAttackUpdate() {
	// playerAttack_->Update();
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
	} else {
		worldTransform_Motion_.at(0).translation_.y = Lerp(
		    0.5f, 0.0f,
		    static_cast<float>(workDash_.dashParameter_) / static_cast<float>(kDashTime));
	}
}

void Player::HitBoxUpdate() {
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
	if (type == static_cast<uint32_t>(Collider::Type::PlayerToEnemy)) {
		// OBB同士が衝突していると仮定して、重なり領域を計算する
		// ここでは、OBB同士の各軸方向での重なりの幅を計算し、最小値を取得する
		Vector3 distance = obb.center_ - obb_.at(0).center_;

		// 当たり判定が成功したので押し戻し処理を行う
		float overlapX = obb_.at(0).size_.x + obb.size_.x - std::abs(distance.x);
		float overlapY = obb_.at(0).size_.y + obb.size_.y - std::abs(distance.y);
		float overlapZ = obb_.at(0).size_.z + obb.size_.z - std::abs(distance.z);

		if (overlapX < overlapY && overlapX < overlapZ) {
			if (distance.x < 0.0f) {
				obb_.at(0).center_ += Vector3{overlapX, 0, 0};
			} else {
				obb_.at(0).center_ += Vector3{-overlapX, 0, 0};
			}
		} else if (overlapY < overlapX && overlapY < overlapZ) {
			if (distance.y < 0.0f) {
				obb_.at(0).center_ += Vector3{0, overlapY, 0};
			} else {
				obb_.at(0).center_ += Vector3{0, -overlapY, 0};
			}
		} else {
			if (distance.z < 0.0f) {
				obb_.at(0).center_ += Vector3{0, 0, overlapZ};
			} else {
				obb_.at(0).center_ += Vector3{0, 0, -overlapZ};
			}
		}
		worldTransform_.at(0).translation_ = obb_.at(0).center_;
		// 転送
		BaseCharacter::Update();
		HitBoxUpdate();
	}
}

void Player::SpriteUpdate() {
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
#pragma region Chage
	{
	// 背景
	Vector2 size = chage_Back_Sprite_->GetSize();
	float t = static_cast<float>(playerAttack_->GetChageAttackCount()) /
	          static_cast<float>(playerAttack_->GetChageAttackCoolTimrCount());
	size.y = Lerp(test_Scale_, 0.0f, t);
	chage_Back_Sprite_->SetSize(size);
	if (t <= 0.0f) {
		// 攻撃できる
		chage_Back_Sprite_->SetColor(Vector4(0.0f, 0.7f, 1.0f, 0.8f));
		chage_Sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		y_Buttan_Sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	} else {
		// できない
		chage_Back_Sprite_->SetColor(Vector4(0.2f, 0.5f, 0.7f, 0.5f));
		chage_Sprite_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
		y_Buttan_Sprite_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	}
	if ((playerAttack_->GetChageAttackFlag()) &&
	    (Input::GetInstance()->TriggerKey(DIK_Q) ||
	     (Input::GetInstance()->GetJoystickState(0, joyState) &&
	      (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)))) {
		chage_Sprite_->SetColor(Vector4(0.8f, 0.0f, 0.0f, 1.0f));
		y_Buttan_Sprite_->SetColor(Vector4(0.8f, 0.0f, 0.0f, 1.0f));
	}
	}
#pragma endregion
#pragma region Triple
	{
	// 背景
	Vector2 size = triple_Back_Sprite_->GetSize();
	float t = static_cast<float>(playerAttack_->GetTripleAttackCount()) /
	          static_cast<float>(playerAttack_->GetTripleCoolTimeAttackCount());
	size.y = Lerp(test_Scale_,0.0f, t);
	triple_Back_Sprite_->SetSize(size);
	if (t <= 0.0f) {
		// 攻撃できる
		triple_Back_Sprite_->SetColor(Vector4(1.0f, 0.2f, 0.0f, 0.8f));
		triple_Sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		b_Buttan_Sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	} else {
		// できない
		triple_Back_Sprite_->SetColor(Vector4(0.8f, 0.3f, 0.3f, 0.5f));
		triple_Sprite_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
		b_Buttan_Sprite_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	}
	if ((playerAttack_->GetTripleAttackFlag()) &&
	    (Input::GetInstance()->TriggerKey(DIK_E) ||
	    (Input::GetInstance()->GetJoystickState(0, joyState) &&
	     (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B)))) {
		triple_Sprite_->SetColor(Vector4(0.8f, 0.0f, 0.0f, 1.0f));
		b_Buttan_Sprite_->SetColor(Vector4(0.8f, 0.0f, 0.0f, 1.0f));
	}
	}
#pragma endregion
	{
	// 背景
	Vector2 size = dash_Back_Sprite_->GetSize();
	float t = static_cast<float>(dash_Count_) / static_cast<float>(kDash_CoolTime_);
	size.y = Lerp(test_Scale_, 0.0f, t);
	dash_Back_Sprite_->SetSize(size);
	if (t <= 0.0f) {
		// 攻撃できる
		dash_Back_Sprite_->SetColor(Vector4(0.4f, 1.0f, 0.0f, 0.8f));
		dash_Sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		x_Buttan_Sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	} else {
		// できない
		dash_Back_Sprite_->SetColor(Vector4(0.1f, 0.8f, 0.1f, 0.5f));
		dash_Sprite_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
		x_Buttan_Sprite_->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	}
	if ((!IsDash_) && (Input::GetInstance()->TriggerKey(DIK_LSHIFT) ||
		               (Input::GetInstance()->GetJoystickState(0, joyState) &&
		                (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X)))) {
		dash_Sprite_->SetColor(Vector4(0.8f, 0.0f, 0.0f, 1.0f));
		x_Buttan_Sprite_->SetColor(Vector4(0.8f, 0.0f, 0.0f, 1.0f));
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

void Player::DrawUI() { 
	center_Back_Sprite_->Draw();

	chage_Back_Black_Sprite_->Draw();
	chage_Back_Sprite_->Draw();
	chage_Sprite_->Draw();
	triple_Back_Black_Sprite_->Draw();
	triple_Back_Sprite_->Draw();
	triple_Sprite_->Draw();
	dash_Back_Black_Sprite_->Draw();
	dash_Back_Sprite_->Draw();
	dash_Sprite_->Draw();

	y_Buttan_Sprite_->Draw();
	b_Buttan_Sprite_->Draw();
	x_Buttan_Sprite_->Draw();

}

void Player::HitBoxInitialize() {
	// AABB
	min_ = {-1.0f, -0.9f, -1.0f};
	max_ = {1.0f, 1.0f, 1.0f};
	// OBB
	size_ = {0.5f, 1.0f, 0.5f};
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

void Player::SetSprite(
    uint32_t chageTextureHandle, uint32_t tripleTextureHandle, uint32_t dashTextureHandle,
    uint32_t whiteTextureHandle, uint32_t y_buttan_TextureHandle, uint32_t b_buttan_TextureHandle,
    uint32_t x_buttan_TextureHandle) {
	chage_Position_ = {test_pos_.x, test_pos_.y - distance_Y};
	triple_Position_ = {test_pos_.x + distance_X, test_pos_.y + distance_Y};
	dash_Position_ = {test_pos_.x - distance_X, test_pos_.y + distance_Y};

	y_Position_ = {test_buttan_pos_.x, test_buttan_pos_.y - distance_buttan_Y};
	b_Position_ = {test_buttan_pos_.x + distance_buttan_X, test_buttan_pos_.y + distance_buttan_Y};
	x_Position_ = {test_buttan_pos_.x - distance_buttan_X, test_buttan_pos_.y + distance_buttan_Y};

	chage_Sprite_ = std::make_unique<Sprite>();
	chage_Back_Sprite_ = std::make_unique<Sprite>();
	chage_Back_Black_Sprite_ = std::make_unique<Sprite>();
	triple_Sprite_ = std::make_unique<Sprite>();
	triple_Back_Sprite_ = std::make_unique<Sprite>();
	triple_Back_Black_Sprite_ = std::make_unique<Sprite>();
	dash_Sprite_ = std::make_unique<Sprite>();
	dash_Back_Sprite_ = std::make_unique<Sprite>();
	dash_Back_Black_Sprite_ = std::make_unique<Sprite>();

	center_Back_Sprite_ = std::make_unique<Sprite>();
	
	x_Buttan_Sprite_ = std::make_unique<Sprite>();
	b_Buttan_Sprite_ = std::make_unique<Sprite>();
	y_Buttan_Sprite_ = std::make_unique<Sprite>();
	
	chage_Sprite_.reset(Sprite::Create(chageTextureHandle, chage_Position_));
	chage_Back_Sprite_.reset(Sprite::Create(
	    whiteTextureHandle, Vector2(chage_Position_.x, chage_Position_.y + test_Scale_),
	    Vector4(0.7f, 0.3f, 0.3f, 0.8f),
	    Vector2(0.0f, 0.0f), false, true));
	chage_Back_Black_Sprite_.reset(Sprite::Create(
	    whiteTextureHandle, Vector2(chage_Position_.x, chage_Position_.y + test_Scale_),
	    Vector4(0.0f, 0.0f, 0.0f, 0.8f), Vector2(0.0f, 0.0f), false, true));
	triple_Sprite_.reset(Sprite::Create(tripleTextureHandle, triple_Position_));
	triple_Back_Sprite_.reset(Sprite::Create(
	    whiteTextureHandle, Vector2(triple_Position_.x, triple_Position_.y+test_Scale_),
	   Vector4(1.0f, 0.2f, 0.0f, 0.8f),
	    Vector2(0.0f, 0.0f), false, true));
	triple_Back_Black_Sprite_.reset(Sprite::Create(
	    whiteTextureHandle, Vector2(triple_Position_.x, triple_Position_.y + test_Scale_),
	    Vector4(0.0f, 0.0f, 0.0f, 0.8f), Vector2(0.0f, 0.0f), false, true));
	dash_Sprite_.reset(Sprite::Create(dashTextureHandle, dash_Position_));
	dash_Back_Sprite_.reset(Sprite::Create(
	    whiteTextureHandle, Vector2(dash_Position_.x, dash_Position_.y + test_Scale_),
	    Vector4(1.0f, 0.0f, 0.0f, 0.5f),
	    Vector2(0.0f, 0.0f), false, true));
	dash_Back_Black_Sprite_.reset(Sprite::Create(
	    whiteTextureHandle, Vector2(dash_Position_.x, dash_Position_.y + test_Scale_),
	    Vector4(0.0f, 0.0f, 0.0f, 0.8f), Vector2(0.0f, 0.0f), false, true));

	center_Back_Sprite_.reset(Sprite::Create(whiteTextureHandle, Vector2(test_pos_.x, test_pos_.y + distance_Y)));

	x_Buttan_Sprite_.reset(Sprite::Create(x_buttan_TextureHandle, x_Position_));
	b_Buttan_Sprite_.reset(Sprite::Create(b_buttan_TextureHandle, b_Position_));
	y_Buttan_Sprite_.reset(Sprite::Create(y_buttan_TextureHandle, y_Position_));

	chage_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	triple_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	dash_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));

	chage_Back_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	triple_Back_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	dash_Back_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));

	chage_Back_Black_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	triple_Back_Black_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	dash_Back_Black_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));
	center_Back_Sprite_->SetSize(Vector2(test_Scale_, test_Scale_));

	x_Buttan_Sprite_->SetSize(Vector2(test_buttan_Scale_,test_buttan_Scale_));
	b_Buttan_Sprite_->SetSize(Vector2(test_buttan_Scale_,test_buttan_Scale_));
	y_Buttan_Sprite_->SetSize(Vector2(test_buttan_Scale_,test_buttan_Scale_));
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
		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
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
	if (Input::GetInstance()->TriggerKey(DIK_Q) ||
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
	}
	// ダッシュ開始
	if ((!IsDash_) && 
		(Input::GetInstance()->TriggerKey(DIK_LSHIFT) ||
	    (Input::GetInstance()->GetJoystickState(0, joyState) &&
	     (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X)))) {
		behaviorRequest_ = Behavior::kDash;
	}
	// ジャンプ
	Jump();
	// 重力
	Gravity();
}

void Player::Move() {
	// 移動量
	vector_ = {0.0f, 0.0f, 0.0f};
#pragma region ゲームパット
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	// ゲームパットの状況取得
	// 入力がなかったら何もしない
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float kMargin = 0.7f;
		// 移動量
		Vector3 move = {
		    static_cast<float>(joyState.Gamepad.sThumbLX),
		    0.0f,
		    static_cast<float>(joyState.Gamepad.sThumbLY),
		};
		if (move.Length() > kMargin) {
			vector_ = {
			    static_cast<float>(joyState.Gamepad.sThumbLX),
			    0.0f,
			    static_cast<float>(joyState.Gamepad.sThumbLY),
			};
		}
	}
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
	velocity_ += acceleration_;
	worldTransform_.at(0).translation_ += velocity_;

	if (std::fabs(velocity_.x) <= 0.001 && std::fabs(velocity_.z) <= 0.001) {
		velocity_.x = 0.0f;
		velocity_.z = 0.0f;
	}
	if (worldTransform_.at(0).translation_.y > kGroundDistanse) {
		acceleration_.y -= kGravity;
	} else {
		acceleration_.y = 0.0f;
		worldTransform_.at(0).translation_.y = kGroundDistanse;
		isJump = false;
	}
}
void Player::ChackTranslation() {
	if (worldTransform_.at(0).translation_.x - worldTransform_.at(0).scale_.x <=
	    -ground_->GetEdge()) {
		worldTransform_.at(0).translation_.x = -ground_->GetEdge() + worldTransform_.at(0).scale_.x;
	} else if (
	    worldTransform_.at(0).translation_.x + worldTransform_.at(0).scale_.x >=
	    ground_->GetEdge()) {
		worldTransform_.at(0).translation_.x = ground_->GetEdge() - worldTransform_.at(0).scale_.x;
	}
	if (worldTransform_.at(0).translation_.z - worldTransform_.at(0).scale_.z <=
	    -ground_->GetEdge()) {
		worldTransform_.at(0).translation_.z = -ground_->GetEdge() + worldTransform_.at(0).scale_.z;
	} else if (
	    worldTransform_.at(0).translation_.z + worldTransform_.at(0).scale_.z >=
	    ground_->GetEdge()) {
		worldTransform_.at(0).translation_.z = ground_->GetEdge() - worldTransform_.at(0).scale_.z;
	}

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
	worldTransform_Motion_.at(0).rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_Motion_.at(0).translation_ = {0.0f, 0.0f, 0.0f};
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