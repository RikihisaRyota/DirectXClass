#pragma once

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

#include <array>
#include <Windows.h>
#include <wrl.h>

#include "Vector2.h"
#include "WinApp.h"

/// <summary>
/// 入力
/// </summary>
class Input {
public: // 静的メンバ関数
	static Input* GetInstans();
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber) const;

	/// <summary>
	/// マウスの押下をチェック
	/// </summary>
	/// <param name="keyNumber">マウスボタン番号(0:左,1:右,2:中)</param>
	/// <returns>押されているか</returns>
	bool PushMouse(int32_t keyNumber) const;

	/// <summary>
	/// キーのトリガーをチェック(押した瞬間true)
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber) const;

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">マウスボタン番号(0:左,1:右,2:中)</param>
	/// <returns>トリガーか</returns>
	bool TriggerMouse(int32_t keyNumber) const;

	/// <summary>
	/// キーを離した瞬間
	/// </summary>
	/// <param name="keyNumber"></param>
	/// <returns></returns>
	bool ExitKey(BYTE keyNumber) const;

	/// <summary>
	/// キーを離した瞬間
	/// </summary>
	/// <param name="keyNumber">マウスボタン番号(0:左,1:右,2:中)</param>
	/// <returns></returns>
	bool ExitMouse(int32_t keyNumber) const;

	/// <summary>
	/// 全キー情報取得
	/// </summary>
	/// <param name="keyStateBuf">全キー情報</param>
	const std::array<BYTE, 256>& GetAllKey() { return key_; }

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	int32_t GetWheel() const;

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Vector2 GetMouseMove() const;
private: // メンバ変数
	Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devKeyboard_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse_;
	DIMOUSESTATE2 mouse_;
	DIMOUSESTATE2 mousePre_;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> keyPre_;
};
