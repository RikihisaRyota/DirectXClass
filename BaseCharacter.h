#pragma once
#include <memory>
#include <vector>

#include "Collider.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
class BaseCharacter {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルデータの配列</param>
	virtual void Initialize(std::vector<std::unique_ptr<Model>> model);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="count"></param>
	//void Resize(size_t count);

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform(size_t count = 0) const {
		return worldTransform_.at(count);
	}
	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform_Motion(size_t count = 0) const {
		return worldTransform_Motion_.at(count);
	}
	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransforms_Parts(int parts, size_t count = 0) const {
		return worldTransforms_Parts_.at(count).at(parts);
	}
	void SetScale(const Vector3& scale, size_t count = 0) { 
		worldTransform_.at(count).scale_ = scale;
		worldTransform_.at(count).UpdateMatrix();
	}
	void SetRotation(const Vector3& rotation, size_t count = 0) {
		worldTransform_.at(count).rotation_ = rotation;
		worldTransform_.at(count).UpdateMatrix();
	}
	void SetTranslation(const Vector3& translation, size_t count = 0) {
		worldTransform_.at(count).translation_ = translation;
		worldTransform_.at(count).UpdateMatrix();
	}
	void SetWorldtransform(const WorldTransform& worldtransfrom, size_t count = 0) {
		worldTransform_.at(count) = worldtransfrom;
		worldTransform_.at(count).UpdateMatrix();
	}
	void SetWorldtransform_Motion(const WorldTransform& worldtransfrom, size_t count = 0) {
		worldTransform_Motion_.at(count) = worldtransfrom;
		worldTransform_Motion_.at(count).UpdateMatrix();
	}
	void SetWorldtransforms_Parts(
	    const WorldTransform& worldtransfrom, int parts, size_t count = 0) {
		worldTransforms_Parts_.at(count).at(parts) = worldtransfrom;
		worldTransforms_Parts_.at(count).at(parts).UpdateMatrix();
	}
	void AddWorldtransform(const WorldTransform& worldtransfrom);

	size_t GetWorldtransformSize() { return worldTransform_.size(); }

	void ResetCount();

protected:
	// モデルデータ配列
	std::vector<std::unique_ptr<Model>> models_;
	// ワールドトランスフォーム
	std::vector<WorldTransform> worldTransform_;
	// ワールドトランスフォーム(アニメーション用)
	std::vector<WorldTransform> worldTransform_Motion_;
	// ワールドトランスフォームパーツごと
	std::vector<std::vector<WorldTransform>> worldTransforms_Parts_;
	// ベクトル
	Vector3 vector_;
	// 速度
	Vector3 velocity_;
	// 加速度
	Vector3 acceleration_;
	// AABB
	Vector3 min_;
	Vector3 max_;
	// OBB
	Vector3 size_;
	Vector3 center_;
	// Sphere
	float radius_;
};