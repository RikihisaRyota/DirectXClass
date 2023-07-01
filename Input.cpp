#include "Input.h"

#include <cassert>
#include <vector>

#include "ImGuiManager.h"
#include "WinApp.h"

#pragma comment(lib, "dinput8.lib")

Input* Input::GetInstans() {
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
