#include "Collider.h"

#include "MyMath.h"

void Collider::HitBoxUpdate() {
	// AABB
	Vector3 worldTranslate = { worldTransform_.at(0).matWorld_.m[3][0] ,worldTransform_.at(0).matWorld_.m[3][1] ,worldTransform_.at(0).matWorld_.m[3][2] };
	aabb_.at(0) = {
		.center_{worldTranslate},
		.min_{worldTranslate + min_},
		.max_{worldTranslate + max_},
	};
	// OBB
	obb_.at(0) = {
		.center_{worldTranslate},
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
		.center_{worldTranslate},
		.radius_{radius_},
	};
}
