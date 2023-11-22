#include "FollowCamera.h"

#include "Enemy.h"
#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Input.h"
#include "TextureManager.h"
void FollowCamera::Intialize() {
	// ビュープロジェクション初期化
	viewProjection_.Initialize();
	// オフセット初期化
	offsetInitialize_ = { 0.0f, 2.0f, -15.0f };
	// オフセット
	offset_ = offsetInitialize_;

	destinationAngle_.x = viewProjection_.rotation_.x;
	destinationAngle_.y = viewProjection_.rotation_.y;

	GlobalVariables* globalVariables = nullptr;
	globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Camera";
	globalVariables->AddItem(groupName, "dashTime", interpolationLate);
	plane_.reset(PlaneRenderer::Create());
	textureHandle_ = TextureManager::Load("resources/reticle.png");
	reticleWorldTarnsform_.Initialize();
}

void FollowCamera::Update() {
	// 追従者がいれば
	if (target_) {
		interTarget_ = Lerp(interTarget_, MakeTranslateMatrix(target_->matWorld_), interpolationLate);
		// 追従対象者からカメラまでのオフセット
		offset_ = offsetInitialize_;
		// ゲームパットのカメラ処理
		GamePad();
		// キーボードのカメラ処理
		Keyboard();
		//// 回転を更新
		RotateUpdate();
		// ビュー行列の更新
		viewProjection_.UpdateMatrix();
	}
//#ifdef _DEBUG
//	ImGui::Begin("camera");
//	ImGui::Text(
//		" viewProjection_.translation_::x:%f,y:%f,z:%f", viewProjection_.translation_.x, viewProjection_.translation_.y,
//		viewProjection_.translation_.z);
//	ImGui::Text(
//		"viewProjection_.rotation_::x:%f,y:%f,z:%f", viewProjection_.rotation_.x, viewProjection_.rotation_.y,
//		viewProjection_.rotation_.z);
//	ImGui::Text(
//		"target_->translation_::x:%f,y:%f,z:%f", target_->translation_.x, target_->translation_.y,
//		target_->translation_.z);
//	ImGui::Text(
//		"target_->rotation_::x:%f,y:%f,z:%f", target_->rotation_.x, target_->rotation_.y, target_->rotation_.z);
//	ImGui::Text("IsTargetCamera  : %d", IsTargetCamera_);
//	ImGui::End();
//#endif // DEBUG
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

void FollowCamera::SetEnemy(Enemy* enemy) {
	enemy_.emplace_back(enemy);
}

Vector3 FollowCamera::OffSet() const {
	Vector3 offset = offsetInitialize_;
	// 回転行列生成
	Matrix4x4 rotate =
		Mul(MakeRotateXMatrix(viewProjection_.rotation_.x),
			MakeRotateYMatrix(viewProjection_.rotation_.y));
	// オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, rotate);
	return offset;
}

void FollowCamera::Draw() {
	if (IsTargetCamera_) {
		plane_->Draw(reticleWorldTarnsform_, viewProjection_, textureHandle_);
	}
}

void FollowCamera::GamePad() {
	if (time_ > 0) {
		time_--;
	}
	// ゲームパットの状態を得る変数
	XINPUT_STATE joyState{};
	// ゲームパットの状況取得
	if (Input::GetInstance()->IsControllerConnected()) {
		Input::GetInstance()->GetJoystickState(0, joyState);
		// カメラの角度から回転行列を計算する
		// X軸
		viewProjection_.rotation_.x -=
			static_cast<float>(joyState.Gamepad.sThumbRY) * kRotateSpeedX;
		viewProjection_.rotation_.x = Clamp(
			viewProjection_.rotation_.x, DegToRad(kDeadZoneRotateMin),
			DegToRad(kDeadZoneRotateMax));
		// Y軸
		viewProjection_.rotation_.y +=
			static_cast<float>(joyState.Gamepad.sThumbRX) * kRotateSpeedY;

		destinationAngle_.y += static_cast<float>(joyState.Gamepad.sThumbRX) * kRotateSpeedY;
		// ターゲットカメラのオンオフ
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
			time_ == 0) {
			IsTargetCamera_ ^= true;
			time_ = 30;
		}
		// プレイヤーの方向を向く
		if (IsTargetCamera_) {
			Vector3 enemyPos{};
			float distance = 9999999.0f;
			for (size_t i = 0; i < 5; i++) {
				if (enemy_.at(i)->GetIsDeathAnimation()) {
					continue;
				}
				float test = (MakeTranslateMatrix(target_->matWorld_) - enemy_.at(i)->GetPosition()).Length();
				if (distance >= test) {
					enemyPos = enemy_.at(i)->GetPosition();
					distance = test;
				}
			}
			if (enemyPos != Vector3({ 0.0f, 0.0f, 0.0f })) {
				Vector3 enemyVector = enemyPos - MakeTranslateMatrix(target_->matWorld_);
				enemyVector.Normalize();
				destinationAngle_.x = enemyVector.x;
				viewProjection_.rotation_.y = Angle({ 0.0f,0.0f,1.0f }, enemyVector);
				reticleWorldTarnsform_.translation_ = enemyPos;
				reticleWorldTarnsform_.scale_ = { 2.0f,2.0f ,2.0f };
				// ビルボード回転行列
				Matrix4x4 bill = MakeBillboardZAxsizLook(
					reticleWorldTarnsform_.translation_,
					viewProjection_.translation_,
					Vector3(0.0f, 1.0f, 0.0f));
				Matrix4x4 worldTransformAffin = MakeAffineMatrix(reticleWorldTarnsform_.scale_, reticleWorldTarnsform_.rotation_, reticleWorldTarnsform_.translation_);
				reticleWorldTarnsform_.matWorld_ = bill * worldTransformAffin;
				reticleWorldTarnsform_.TransferMatrix();
			}
		}
		else {
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
				destinationAngle_.x = target_->rotation_.x;
				destinationAngle_.y = target_->rotation_.y;
			}
		}
	
	}
}

void FollowCamera::Keyboard() {
	if (Input::GetInstance()->PushKey(DIK_RIGHTARROW)) {
		// Y軸を軸に回転
		viewProjection_.rotation_.y += kRotateSpeedY * kKeyboardCameraSpeed_X;
		destinationAngle_.y += kRotateSpeedY * kKeyboardCameraSpeed_X;
	}
	if (Input::GetInstance()->PushKey(DIK_LEFTARROW)) {
		// Y軸を軸に回転
		viewProjection_.rotation_.y -= kRotateSpeedY * kKeyboardCameraSpeed_X;
		destinationAngle_.y -= kRotateSpeedY * kKeyboardCameraSpeed_X;
	}

	if (Input::GetInstance()->PushKey(DIK_UPARROW)) {
		// X軸を軸に回転
		viewProjection_.rotation_.x += kRotateSpeedX * kKeyboardCameraSpeed_Y;
		viewProjection_.rotation_.x = Clamp(
			viewProjection_.rotation_.x, DegToRad(kDeadZoneRotateMin),
			DegToRad(kDeadZoneRotateMax));
		destinationAngle_.x += kRotateSpeedY * kKeyboardCameraSpeed_X;
	}
	if (Input::GetInstance()->PushKey(DIK_DOWNARROW)) {
		// X軸を軸に回転
		viewProjection_.rotation_.x -= kRotateSpeedX * kKeyboardCameraSpeed_Y;
		viewProjection_.rotation_.x = Clamp(
			viewProjection_.rotation_.x, DegToRad(kDeadZoneRotateMin),
			DegToRad(kDeadZoneRotateMax));
		destinationAngle_.x -= kRotateSpeedY * kKeyboardCameraSpeed_X;
	}

	if (Input::GetInstance()->PushKey(DIK_TAB)) {
		destinationAngle_.x = target_->rotation_.x;
		destinationAngle_.y = target_->rotation_.y;
	}
}

void FollowCamera::RotateUpdate() {
	// 最短角度補間
	viewProjection_.rotation_.y =
		LenpShortAngle(viewProjection_.rotation_.y, destinationAngle_.y, interpolationLate);
	// 回転行列生成
	Matrix4x4 rotate =
		Mul(MakeRotateXMatrix(viewProjection_.rotation_.x),
			MakeRotateYMatrix(viewProjection_.rotation_.y));
	// オフセットをカメラの回転に合わせて回転させる
	offset_ = TransformNormal(offset_, rotate);
	// 座標をコピーしてずらす/*interTarget_, target_->translation_*/
	viewProjection_.translation_ = interTarget_ + offset_;

}

void FollowCamera::Reset() {
	// 追従対象がいれば
	if (target_) {
		// 追従座標・角度の初期化
		interTarget_ = MakeTranslateMatrix(target_->matWorld_);
		viewProjection_.rotation_.y = target_->rotation_.y;
	}
	destinationAngle_.x = viewProjection_.rotation_.x;
	destinationAngle_.y = viewProjection_.rotation_.y;

	// 追従対象からオフセット
	Vector3 offset = OffSet();
	viewProjection_.translation_ = interTarget_ + offset;
}

void FollowCamera::GetGlobalVariables() {
	GlobalVariables* globalVariables = nullptr;
	globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Camera";
	globalVariables->LoadFiles();
}
