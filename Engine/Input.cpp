#include "Input.h"

#include <cassert>
#include <vector>


#include "ImGuiManager.h"
#include "Matrix4x4.h"
#include "MyMath.h"
#include "ViewProjection.h"
#include "Vector2.h"
#include "Vector3.h"
#include "WinApp.h"

// デバイス発見時に実行される
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef) {
	return DIENUM_CONTINUE;
}

Input* Input::GetInstance() {
	static Input instans;
	return &instans;
}

void Input::Initialize() {
	HRESULT result = S_FALSE;
#pragma region DirectInputオブジェクトの生成
	// DirectInputオブジェクトの生成
	result = DirectInput8Create(
		WinApp::GetInstance()->GethInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(dInput_.GetAddressOf()), nullptr);
	assert(SUCCEEDED(result));
#pragma endregion DirectInputオブジェクトの生成

#pragma region キーボード設定
	// キーボードデバイスの生成
	result = dInput_->CreateDevice(GUID_SysKeyboard, &devKeyboard_, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット(キーボード)
	result = devKeyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result =
		devKeyboard_->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion キーボード設定

#pragma region マウス設定
	// マウスデバイスの生成
	result = dInput_->CreateDevice(GUID_SysMouse, &devMouse_, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = devMouse_->SetDataFormat(&c_dfDIMouse); // マウス用のデータ・フォーマットを設定
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result =
		devMouse_->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion マウス設定
#pragma region ジョイスティック
	/*result = dInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
	assert(SUCCEEDED(result));*/

	// XInput 初期化
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
		Joystick joystick;
		joystick.type_ = PadType::XInput;
		joystick.device_ = nullptr; // XInput では DirectInput デバイスは使用しない
		devJoysticks_.push_back(joystick);
	}
#pragma endregion
}

void Input::Update() {
	devKeyboard_->Acquire(); // キーボード動作開始
	devMouse_->Acquire(); // マウス動作開始
	// 前回のキー入力を保存
	keyPre_ = key_;

	mousePre_ = mouse_;

	// キーの入力
	devKeyboard_->GetDeviceState((DWORD)size(key_), key_.data());

	// マウスの入力
	devMouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_);

	// ジョイスティックの状態を更新
	for (DWORD i = 0; i < devJoysticks_.size(); ++i) {
		if (devJoysticks_[i].type_ == PadType::DirectInput) {
			devJoysticks_[i].device_->Acquire();
			devJoysticks_[i].device_->GetDeviceState(sizeof(DIJOYSTATE2), &devJoysticks_[i].state_.directInput_);
		}
		else if (devJoysticks_[i].type_ == PadType::XInput) {
			XINPUT_STATE xInputState;
			ZeroMemory(&xInputState, sizeof(XINPUT_STATE));
			DWORD result = XInputGetState(i, &xInputState);
			if (result == ERROR_SUCCESS) {
				devJoysticks_[i].state_.xInput_ = xInputState;
				// Zero value if thumbsticks are within the dead zone
				if ((devJoysticks_[i].state_.xInput_.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					devJoysticks_[i].state_.xInput_.Gamepad.sThumbLX  > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
					(devJoysticks_[i].state_.xInput_.Gamepad.sThumbLY  <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
						devJoysticks_[i].state_.xInput_.Gamepad.sThumbLY  > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
					devJoysticks_[i].state_.xInput_.Gamepad.sThumbLX = 0;
					devJoysticks_[i].state_.xInput_.Gamepad.sThumbLY = 0;
				}
				if ((devJoysticks_[i].state_.xInput_.Gamepad.sThumbRX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					devJoysticks_[i].state_.xInput_.Gamepad.sThumbRX  > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
					(devJoysticks_[i].state_.xInput_.Gamepad.sThumbRY  <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
						devJoysticks_[i].state_.xInput_.Gamepad.sThumbRY  > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
					devJoysticks_[i].state_.xInput_.Gamepad.sThumbRX = 0;
					devJoysticks_[i].state_.xInput_.Gamepad.sThumbRY = 0;
				}
			}
		}
	}
}

bool Input::PushKey(BYTE keyNumber) const {

	// 0でなければ押している
	if (key_[keyNumber]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouse(int32_t keyNumber) const {
	if (mouse_.rgbButtons[keyNumber]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)const {
	// 前回が0で、今回が0でなければトリガー
	if (!keyPre_[keyNumber] && key_[keyNumber]) {
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouse(int32_t keyNumber) const {
	// 前回が0で、今回が0でなければトリガー
	if (!mousePre_.rgbButtons[keyNumber] && mouse_.rgbButtons[keyNumber]) {
		return true;
	}
	// トリガーでない
	return false;
}

bool Input::ExitKey(BYTE keyNumber) const {
	// 前回が0ではなくて、今回が0
	if (keyPre_[keyNumber] && !key_[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::ExitMouse(int32_t keyNumber) const {
	// 前回が0ではなくて、今回が0
	if (mousePre_.rgbButtons[keyNumber] && !mouse_.rgbButtons[keyNumber]) {
		return true;
	}
	return false;
}

int32_t Input::GetWheel() const {
	return static_cast<int32_t>(mouse_.lZ);
}

Vector2 Input::GetMouseMove() const {
	return {(float)mouse_.lX,(float)mouse_.lY};
}

Vector2 Input::GetMouseScreenPosition() const {
	POINT cursorPos;
	GetCursorPos(&cursorPos); // カーソルのスクリーン上の位置を取得
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &cursorPos); // スクリーン座標をクライアント座標に変換

	return Vector2(static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y));
}

Vector3 Input::GetMouseWorldPosition(float cameraDistance, const ViewProjection& viewProjection) const {
	POINT cursorPos;
	GetCursorPos(&cursorPos); // カーソルのスクリーン上の位置を取得
	ScreenToClient(WinApp::GetInstance()->GetHwnd(), &cursorPos); // スクリーン座標をクライアント座標に変換

	Vector2 screenPos (static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y));
	// ビュー行列
	Matrix4x4 matView = viewProjection.matView_;
	// ビューポート行列
	Matrix4x4 matViewProjection = viewProjection.matProjection_;
	// ビューポート行列
	Matrix4x4 matViewport =
		MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matVPV = matView * matViewProjection * matViewport;

	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3(
		static_cast<float>(screenPos.x), static_cast<float>(screenPos.y), 0.0f);

	Vector3 posFar = Vector3(
		static_cast<float>(screenPos.x), static_cast<float>(screenPos.y), 1.0f);

	// スクリーン座標系からワールド座標系
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;

	mouseDirection.Normalize();

	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = cameraDistance;

	Vector3 reticlePos = posNear + (mouseDirection * kDistanceTestObject);

	return Vector3(reticlePos.x, reticlePos.y, reticlePos.z);
}

bool Input::GetJoystickState(int32_t stickNo, DIJOYSTATE2& out) const {
	if (stickNo >= 0 && stickNo < static_cast<int32_t>(devJoysticks_.size())) {
		out = devJoysticks_[stickNo].state_.directInput_;
		return true;
	}
	return false;
}

bool Input::GetJoystickStatePrevious(int32_t stickNo, DIJOYSTATE2& out) const {
	if (stickNo >= 0 && stickNo < static_cast<int32_t>(devJoysticks_.size())) {
		out = devJoysticks_[stickNo].statePre_.directInput_;
		return true;
	}
	return false;
}

bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const {
	if (stickNo >= 0 && stickNo < static_cast<int32_t>(devJoysticks_.size())) {
		if (devJoysticks_[stickNo].type_ == PadType::XInput) {
			out = devJoysticks_[stickNo].state_.xInput_;
			return true;
		}
		else {
			// ジョイスティックが接続されていない場合
			return false;
		}
	}
	return false;
}


bool Input::GetJoystickStatePrevious(int32_t stickNo, XINPUT_STATE& out) const {
	if (stickNo >= 0 && stickNo < static_cast<int32_t>(devJoysticks_.size())) {
		if (devJoysticks_[stickNo].type_ == PadType::XInput) {
			out = devJoysticks_[stickNo].statePre_.xInput_;
			return true;
		}
	}
	return false;
}
//BOOL CALLBACK Input::EnumJoystickObjectsCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context) {
//	IDirectInputDevice8* device = static_cast<IDirectInputDevice8*>(context);
//
//	DIPROPRANGE range;
//	range.diph.dwSize = sizeof(DIPROPRANGE);
//	range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
//	range.diph.dwObj = instance->dwType;
//	range.diph.dwHow = DIPH_BYID;
//	range.lMin = -1000;
//	range.lMax = 1000;
//
//	HRESULT result = device->SetProperty(DIPROP_RANGE, &range.diph);
//	if (FAILED(result)) {
//		return DIENUM_STOP;
//	}
//
//	return DIENUM_CONTINUE;
//}
//
//BOOL CALLBACK Input::EnumJoysticksCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
//
//	Joystick joystick;
//	joystick.type_ = PadType::DirectInput;
//	joystick.device_ = nullptr;
//	joystick.state_ = {};
//	joystick.statePre_ = {};
//
//	HRESULT result = dInput_->CreateDevice(instance->guidInstance, &joystick.device_, nullptr);
//	if (SUCCEEDED(result)) {
//		result = joystick.device_->SetDataFormat(&c_dfDIJoystick);
//		if (SUCCEEDED(result)) {
//			result = joystick.device_->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
//			if (SUCCEEDED(result)) {
//				// オブジェクトの列挙と設定
//				result = joystick.device_->EnumObjects(EnumJoystickObjectsCallback, joystick.device_.Get(), DIDFT_ALL);
//				if (SUCCEEDED(result)) {
//					devJoysticks_.push_back(joystick);
//				}
//			}
//		}
//	}
//
//	return DIENUM_CONTINUE;
//}
