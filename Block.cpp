#include "Block.h"

#include "Draw.h"
#include "MyMath.h"

void Block::Initialize(std::vector<std::unique_ptr<Model>> model) {
	BaseCharacter::Initialize(std::move(model));
	worldTransform_.at(0).translation_.y =-10.0f;
	worldTransform_.at(0).scale_ = {10.0f,10.0f,10.0f};

	BaseCharacter::Update();

	HitBoxInitialize(kCollisionAttributeBlock);
}

void Block::Update() {}

void Block::Draw(const ViewProjection& viewProjection) {
	models_.at(0)->Draw(worldTransform_.at(0),viewProjection);
}

void Block::OnCollision(const OBB& obb, uint32_t type) {
	OBB o = obb;
	uint32_t i = type;
}

void Block::HitBoxInitialize(uint32_t collisionMask) {
	// 衝突属性を設定
	SetCollisionAttribute(collisionMask);
	// 衝突対象を自分以外に設定
	SetCollisionMask(~collisionMask);
	// AABB
	aabb_.resize(1);
	obb_.resize(1);

	// AABB
	min_ = { -worldTransform_.at(0).scale_ };
	max_ = { worldTransform_.at(0).scale_ };
	// OBB
	size_ = worldTransform_.at(0).scale_;
	// Sphere
	radius_ = 1.2f;
	// AABB
	aabb_.at(0) = {
		.center_{worldTransform_.at(0).translation_},
		.min_{worldTransform_.at(0).translation_ + min_},
		.max_{worldTransform_.at(0).translation_ + max_},
	};
	// OBB
	obb_.at(0) = {
		.center_{ worldTransform_.at(0).translation_},
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

void Block::HitBoxUpdate() {

}

void Block::HitBoxDraw(const ViewProjection& viewProjection) {
	DrawAABB(aabb_.at(0), viewProjection, Vector4(0.0f, 0.5f, 0.25f, 1.0f));
	DrawOBB(obb_.at(0), viewProjection, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}
