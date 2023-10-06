#include "Block.h"

#include "Draw.h"
#include "MyMath.h"
#include "ImGuiManager.h"

void Block::Initialize(std::vector<std::unique_ptr<Model>> model) {
	BaseCharacter::Initialize(std::move(model));
	for (size_t i = 0; i < 5; ++i) {
		WorldTransform worldtransform{};
		worldtransform.Initialize();
		BaseCharacter::AddWorldtransform(worldtransform);
	}
	float scale = 5.0f;
	worldTransform_.at(0).translation_ = { 0.0f,-scale,0.0f };
	worldTransform_.at(0).scale_ = { scale ,scale ,10.0f };

	worldTransform_.at(1).translation_ = { scale * 3,-scale,scale};
	worldTransform_.at(1).scale_ = { scale ,scale ,scale };

	worldTransform_.at(2).translation_ = { scale * 6,-scale,scale };
	worldTransform_.at(2).scale_ = { scale ,scale ,scale };

	worldTransform_.at(3).translation_ = { scale * 6.0f,-scale,scale * 4.0f };
	worldTransform_.at(3).scale_ = { scale ,scale ,scale };

	worldTransform_.at(4).translation_ = { scale * 6.0f,-scale,scale * 7.0f };
	worldTransform_.at(4).scale_ = { scale ,scale ,scale };

	worldTransform_.at(5).translation_ = { scale * 9.0f,-scale,scale * 7.0f };
	worldTransform_.at(5).scale_ = { scale ,scale ,scale };

	BaseCharacter::Update();

	HitBoxInitialize(kCollisionAttributeBlock);
}

void Block::Update() {
	ImGui::Begin("Block");
	ImGui::SliderFloat3("pos", &worldTransform_.at(0).translation_.x, -10.0f, 10.0f);
	ImGui::End();
	BaseCharacter::Update();
	HitBoxUpdate();
}

void Block::Draw(const ViewProjection& viewProjection) {
	for (size_t i = 0; i < worldTransform_.size(); i++) {
		models_.at(0)->Draw(worldTransform_.at(i), viewProjection);
	}
}

void Block::OnCollision(const OBB& obb, const WorldTransform& worldTransform, uint32_t type) {
	OBB o = obb;
	uint32_t i = type;

}

void Block::HitBoxInitialize(uint32_t collisionMask) {
	// 衝突属性を設定
	SetCollisionAttribute(collisionMask);
	// 衝突対象を自分以外に設定
	SetCollisionMask(~collisionMask);
	aabb_.resize(worldTransform_.size());
	obb_.resize(worldTransform_.size());
	for (size_t i = 0; i < worldTransform_.size(); i++) {
		// AABB
		min_ = { -worldTransform_.at(i).scale_ };
		max_ = { worldTransform_.at(i).scale_ };
		// OBB
		size_ = worldTransform_.at(i).scale_;
		// Sphere
		radius_ = 1.2f;
		// AABB
		aabb_.at(i) = {
			.center_{worldTransform_.at(i).translation_},
			.min_{worldTransform_.at(i).translation_ + min_},
			.max_{worldTransform_.at(i).translation_ + max_},
		};
		// OBB
		obb_.at(i) = {
			.center_{ worldTransform_.at(i).translation_},
			.orientations_{
					 {1.0f, 0.0f, 0.0f},
					 {0.0f, 1.0f, 0.0f},
					 {0.0f, 0.0f, 1.0f},
					 },
			.size_{size_}
		};
		obb_.at(i) = OBBSetRotate(obb_.at(i), worldTransform_.at(i).rotation_);
		// Sphere
		sphere_ = {
			.center_{worldTransform_.at(i).translation_},
			.radius_{size_.x},
		};
	}

}

void Block::HitBoxUpdate() {
	for (size_t i = 0; i < worldTransform_.size(); i++) {
		// AABB
		min_ = { -worldTransform_.at(i).scale_ };
		max_ = { worldTransform_.at(i).scale_ };
		// OBB
		size_ = worldTransform_.at(i).scale_;
		// Sphere
		radius_ = 1.2f;
		// AABB
		aabb_.at(i) = {
			.center_{worldTransform_.at(i).translation_},
			.min_{worldTransform_.at(i).translation_ + min_},
			.max_{worldTransform_.at(i).translation_ + max_},
		};
		// OBB
		obb_.at(i) = {
			.center_{ worldTransform_.at(i).translation_},
			.orientations_{
					 {1.0f, 0.0f, 0.0f},
					 {0.0f, 1.0f, 0.0f},
					 {0.0f, 0.0f, 1.0f},
					 },
			.size_{size_}
		};
		obb_.at(i) = OBBSetRotate(obb_.at(i), worldTransform_.at(i).rotation_);
		// Sphere
		sphere_ = {
			.center_{worldTransform_.at(i).translation_},
			.radius_{size_.x},
		};
	}
}

void Block::HitBoxDraw(const ViewProjection& viewProjection) {
	for (size_t i = 0; i < worldTransform_.size(); i++) {
		DrawAABB(aabb_.at(0), viewProjection, Vector4(0.0f, 0.5f, 0.25f, 1.0f));
		DrawOBB(obb_.at(0), viewProjection, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}
