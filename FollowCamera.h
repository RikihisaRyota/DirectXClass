#pragma once

#include "Vector2.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class FollowCamera {
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Intialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

public: // セッター,ゲッター
	/// <summary>
	/// ターゲット
	/// </summary>
	/// <param name="target">target</param>
	void SetTarget(const WorldTransform* target);
	/// <summary>
	/// 敵へのターゲットカメラ
	/// </summary>
	/// <param name="target"></param>
	void SetEnemy(const WorldTransform* target);
	/// <summary>
	/// offsetを
	/// </summary>
	/// <param name="target">target</param>
	void SetOffset(const Vector3 offset) { offset_ = offset; }
	/// <summary>
	/// ViewProjectionのゲッター
	/// </summary>
	/// <returns></returns>
	ViewProjection* GetViewProjection() { return &viewProjection_; }

	bool GetIsTargetCamera() const { return IsTargetCamera_; }

private:
	/// <summary>
	/// ゲームパットのカメラ処理
	/// </summary>
	void GamePad();
	/// <summary>
	/// キーボードのカメラ処理
	/// </summary>
	void Keyboard();
	/// <summary>
	/// 回転を更新
	/// </summary>
	void RotateUpdate();

	/// <summary>
	/// 追従対象からのオフセットを計算する
	/// </summary>
	/// <returns></returns>
	Vector3 OffSet() const;

	void Reset();

private:
	// 回転速度
	const float kRotateSpeedX = 0.000001f;
	const float kRotateSpeedY = 0.000002f;
	// 上下方向の回転のデッドゾーン
	const float kDeadZoneRotateMin = -10.0f;
	const float kDeadZoneRotateMax = 70.0f;
	// キーボードカメラスピード
	const float kKeyboardCameraSpeed_Y = 10000.0f;
	const float kKeyboardCameraSpeed_X = 15000.0f;

private: // メンバ変数
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// 追従対象
	const WorldTransform* target_ = nullptr;
	// 追従対象
	const WorldTransform* enemy_ = nullptr;
	// 追従対象の残像
	Vector3 interTarget_ = {};
	// オフセット(いらない)
	Vector3 offset_;
	// オフセット初期化
	Vector3 offsetInitialize_;
	// 目標角度
	Vector2 destinationAngle_;
	// ターゲットカメラ
	bool IsTargetCamera_ = false;
};