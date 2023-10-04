#pragma once
#include <cstdint>
#include <vector>

#include "AABB.h"
#include "Sphere.h"
#include "OBB.h"
#include "ViewProjection.h"

// プレイヤー陣営
const uint32_t kCollisionAttributePlayer = 0b1;
// 敵陣営
const uint32_t kCollisionAttributeEnemy = 0b1 << 1;
// 敵の攻撃属性
const uint32_t kCollisionAttributeEnemyAttack = 0b1 << 2;
// プレイヤーの攻撃属性
const uint32_t kCollisionAttributePlayerAttack = 0b1 << 3;

struct Vector3;
/// <summary>
/// 衝突判定オブジェクト
/// </summary>
class Collider {
public:
	enum class Type {
		PlayerToEnemy,
		PlayerToEnemyAttack,
		EnemyToPlayerAttack,
	};

	// 衝突時に呼ばれる関数
	virtual void OnCollision(const OBB& obb , uint32_t type) = 0;

	// 衝突属性の取得
	virtual uint32_t GetCollisionAttribute() { return collisionAttribute_; }

	// 衝突属性設定
	virtual void SetCollisionAttribute(uint32_t collisionAttribute) {
		collisionAttribute_ = collisionAttribute;
	}

	// 衝突マスクの取得
	virtual uint32_t GetCollisionMask() { return collisionAttribute_; }

	// 衝突マスク設定
	virtual void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; }

	// HitBoxDraw
	virtual void HitBoxInitialize() = 0;
	virtual void HitBoxUpdate() = 0;
	virtual void HitBoxDraw(const ViewProjection& viewProjection) = 0;

	AABB* GetAABB(size_t index = 0) { return &aabb_.at(index); }
	OBB* GetOBB(size_t index = 0) { return &obb_.at(index); }
	size_t GetAABBSize() { return aabb_.size(); }
	size_t GetOBBSize() { return obb_.size(); }

protected:
	// AABB
	std::vector<AABB> aabb_;
	// OBB
	std::vector<OBB> obb_;
	// Sphere
	Sphere sphere_;
	// 衝突属性(自分)
	uint32_t collisionAttribute_ = 0xffffffff;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xffffffff;
};
