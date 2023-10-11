#include "Collider.h"

#include "MyMath.h"

void Collider::HitBoxUpdate() {
	// AABB
	aabb_.at(0) = {
		.center_{worldTransform_.at(0).translation_},
		.min_{worldTransform_.at(0).translation_ + min_},
		.max_{worldTransform_.at(0).translation_ + max_},
	};
	// OBB
	obb_.at(0) = {
		.center_{worldTransform_.at(0).translation_},
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
