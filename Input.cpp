#include "Input.h"

#include <cassert>
#include <vector>

#include "ImGuiManager.h"
#include "WinApp.h"

Input* Input::GetInstance() {
	static Input instans;
	return &instans;
}

void Input::Initialize() {
	HRESULT result = S_FALSE;
#pragma region DirectInputオブジェクトの生成
	// DirectInputオブジェクトの生成
	result = DirectInput8Create(
		WinApp::GetInstance()->GethInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput_, nullptr);
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
	// ジョイスティックデバイスの列挙と初期化
	for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
		Joystick joystick;
		joystick.type_ = PadType::XInput;
		joystick.state_ = {};

		// XInput ジョイスティックの初期化処理
		XINPUT_STATE xInputState;
		ZeroMemory(&xInputState, sizeof(XINPUT_STATE));

		DWORD result = XInputGetState(i, &xInputState);
		if (result == ERROR_SUCCESS) {
			joystick.device_ = nullptr; // XInput では DirectInput デバイスは使用しない
			joystick.state_.xInput_ = xInputState;
			joystick.statePre_.xInput_ = xInputState;
		}

		if (SUCCEEDED(result)) {
			devJoysticks_.emplace_back(joystick);
		}
	}
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
