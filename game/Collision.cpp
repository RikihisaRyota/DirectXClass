#include "Collision.h"

#include <algorithm>
#include <math.h>

#include "MyMath.h"

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	// 2つの球の中心間の距離を求める
	float distance = Length(Subtract(s1.center_, s2.center_));
	if (distance <= s1.radius_ + s2.radius_) {
		return true;
	}
	return false;
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	float k = Dot(plane.normal_, sphere.center_) - plane.distanse_;
	if (std::abs(k) <= sphere.radius_) {
		return true;
	}
	return false;
}

bool IsCollision(const Plane& plane, const Segment& segment) {
	// 垂直判定
	float d = Dot(plane.normal_, segment.diff);
	// 平行だったら
	if (d == 0.0f) {
		return false;
	}
	// 線分と平面の当たり判定
	float t = (plane.distanse_ - Dot(segment.origin, plane.normal_)) / d;
	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const Plane& plane, const Ray& ray) {
	// 垂直判定
	float d = Dot(plane.normal_, ray.diff);
	// 平行だったら
	if (d == 0.0f) {
		return false;
	}
	// 半直線と平面の当たり判定
	float t = (plane.distanse_ - Dot(ray.origin, plane.normal_)) / d;
	if (t >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const Plane& plane, const Line& line) {
	// 垂直判定
	float d = Dot(plane.normal_, line.diff);
	// 直線
	if (d == 0.0f) {
		return false;
	}
	return true;
}

bool IsCollision(const Triangle& triangle, const Segment& segment) {
	// 各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積をとる
	Vector3 v01 = triangle.vertices_[1] - triangle.vertices_[0];
	Vector3 v12 = triangle.vertices_[2] - triangle.vertices_[1];
	Vector3 v20 = triangle.vertices_[0] - triangle.vertices_[2];
	Vector3 normal = Cross(v01, v12);
	normal = Normalize(normal);

	// 垂直判定
	float d = Dot(normal, segment.diff);
	// 平行なら当たらない
	if (d == 0.0f) {
		return false;
	}
	float distance = Dot(triangle.vertices_[0], normal);
	// 線分と平面の当たり判定
	float t = (distance - Dot(segment.origin, normal)) / d;
	// 当り判定用の平面
	Plane tmp{.normal_{normal}, .distanse_{distance}};
	if (!IsCollision(tmp, segment)) {
		return false;
	}
	Vector3 intersectionPoint = segment.origin + (segment.diff * t);
	Vector3 v0p = intersectionPoint - triangle.vertices_[0];
	Vector3 v1p = intersectionPoint - triangle.vertices_[1];
	Vector3 v2p = intersectionPoint - triangle.vertices_[2];

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	// 全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, normal) >= 0.0f && Dot(cross12, normal) >= 0.0f &&
	    Dot(cross20, normal) >= 0.0f) {
		// 衝突
		return true;
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Ray& ray) {
	// 各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積をとる
	Vector3 v01 = triangle.vertices_[1] - triangle.vertices_[0];
	Vector3 v12 = triangle.vertices_[2] - triangle.vertices_[1];
	Vector3 v20 = triangle.vertices_[0] - triangle.vertices_[2];
	Vector3 normal = Cross(v01, v12);
	normal = Normalize(normal);

	// 垂直判定
	float d = Dot(normal, ray.diff);
	// 平行なら当たらない
	if (d == 0.0f) {
		return false;
	}
	float distance = Dot(triangle.vertices_[0], normal);
	// 半直線と平面の当たり判定
	float t = (distance - Dot(ray.origin, normal)) / d;
	// 当り判定用の平面
	Plane tmp{.normal_{normal}, .distanse_{distance}};
	if (!IsCollision(tmp, ray)) {
		return false;
	}
	Vector3 intersectionPoint = ray.origin + (ray.diff * t);
	Vector3 v0p = intersectionPoint - triangle.vertices_[0];
	Vector3 v1p = intersectionPoint - triangle.vertices_[1];
	Vector3 v2p = intersectionPoint - triangle.vertices_[2];

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	// 全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, normal) >= 0.0f && Dot(cross12, normal) >= 0.0f &&
	    Dot(cross20, normal) >= 0.0f) {
		// 衝突
		return true;
	}
	return false;
}

bool IsCollision(const Triangle& triangle, const Line& line) {
	// 各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積をとる
	Vector3 v01 = triangle.vertices_[1] - triangle.vertices_[0];
	Vector3 v12 = triangle.vertices_[2] - triangle.vertices_[1];
	Vector3 v20 = triangle.vertices_[0] - triangle.vertices_[2];
	Vector3 normal = Cross(v01, v12);
	normal = Normalize(normal);

	// 垂直判定
	float d = Dot(normal, line.diff);
	// 平行なら当たらない
	if (d == 0.0f) {
		return false;
	}
	float distance = Dot(triangle.vertices_[0], normal);
	// 半直線と平面の当たり判定
	float t = (distance - Dot(line.origin, normal)) / d;
	// 当り判定用の平面
	Plane tmp{.normal_{normal}, .distanse_{distance}};
	if (!IsCollision(tmp, line)) {
		return false;
	}
	Vector3 intersectionPoint = line.origin + (line.diff * t);
	Vector3 v0p = intersectionPoint - triangle.vertices_[0];
	Vector3 v1p = intersectionPoint - triangle.vertices_[1];
	Vector3 v2p = intersectionPoint - triangle.vertices_[2];

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	// 全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, normal) >= 0.0f && Dot(cross12, normal) >= 0.0f &&
	    Dot(cross20, normal) >= 0.0f) {
		// 衝突
		return true;
	}
	return false;
}

bool IsCollision(const AABB& a, const AABB& b) {
	if ((a.min_.x <= b.max_.x && a.max_.x >= b.min_.x) && // x軸
	    (a.min_.y <= b.max_.y && a.max_.y >= b.min_.y) && // y軸
	    (a.min_.z <= b.max_.z && a.max_.z >= b.min_.z)    // z軸
	) {
		// 衝突
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	// 最近接点を求める
	Vector3 clossPoint{
	    std::clamp(sphere.center_.x, aabb.min_.x, aabb.max_.x),
	    std::clamp(sphere.center_.y, aabb.min_.y, aabb.max_.y),
	    std::clamp(sphere.center_.z, aabb.min_.z, aabb.max_.z),
	};
	// 最近接点と球の中心との距離を求める
	float distance = Length(clossPoint - sphere.center_);
	// 衝突が半径よりも小さければ衝突
	if (distance <= sphere.radius_) {
		// 衝突
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Segment& segment) {
	// X軸
	float txMin = (aabb.min_.x - segment.origin.x) / segment.diff.x;
	float txMax = (aabb.max_.x - segment.origin.x) / segment.diff.x;
	float tNearX = (std::min)(txMin, txMax);
	float tFarX = (std::max)(txMin, txMax);
	// Y軸
	float tyMin = (aabb.min_.y - segment.origin.y) / segment.diff.y;
	float tyMax = (aabb.max_.y - segment.origin.y) / segment.diff.y;
	float tNearY = (std::min)(tyMin, tyMax);
	float tFarY = (std::max)(tyMin, tyMax);
	// Z軸
	float tzMin = (aabb.min_.z - segment.origin.z) / segment.diff.z;
	float tzMax = (aabb.max_.z - segment.origin.z) / segment.diff.z;
	float tNearZ = (std::min)(tzMin, tzMax);
	float tFarZ = (std::max)(tzMin, tzMax);
	// 各軸のtの最小値と最大値を計算する
	// AABBと衝突点（貫通点）のtが小さい方
	float tMin = (std::max)((std::max)(tNearX, tNearY), tNearZ);
	// AABBと衝突点（貫通点）のtが大きい方
	float tMax = (std::min)((std::min)(tFarX, tFarY), tFarZ);
	if ((tMin < 0.0f && tMax < 0.0f) || (tMin > 1.0f && tMax > 1.0f)) {
		return false;
	}
	if (tMin > tMax) {
		return false;
	}
	return true;
}

bool IsCollision(const AABB& aabb, const Ray& ray) {
	// X軸
	float txMin = (aabb.min_.x - ray.origin.x) / ray.diff.x;
	float txMax = (aabb.max_.x - ray.origin.x) / ray.diff.x;
	float tNearX = (std::min)(txMin, txMax);
	float tFarX = (std::max)(txMin, txMax);
	// Y軸
	float tyMin = (aabb.min_.y - ray.origin.y) / ray.diff.y;
	float tyMax = (aabb.max_.y - ray.origin.y) / ray.diff.y;
	float tNearY = (std::min)(tyMin, tyMax);
	float tFarY = (std::max)(tyMin, tyMax);
	// Z軸
	float tzMin = (aabb.min_.z - ray.origin.z) / ray.diff.z;
	float tzMax = (aabb.max_.z - ray.origin.z) / ray.diff.z;
	float tNearZ = (std::min)(tzMin, tzMax);
	float tFarZ = (std::max)(tzMin, tzMax);
	// 各軸のtの最小値と最大値を計算する
	// AABBと衝突点（貫通点）のtが小さい方
	float tMin = (std::max)((std::max)(tNearX, tNearY), tNearZ);
	// AABBと衝突点（貫通点）のtが大きい方
	float tMax = (std::min)((std::min)(tFarX, tFarY), tFarZ);
	if ((tMin < 0.0f && tMax < 0.0f)) {
		return false;
	}
	if (tMin > tMax) {
		return false;
	}
	return true;
}

bool IsCollision(const AABB& aabb, const Line& line) {
	// X軸
	float txMin = (aabb.min_.x - line.origin.x) / line.diff.x;
	float txMax = (aabb.max_.x - line.origin.x) / line.diff.x;
	float tNearX = (std::min)(txMin, txMax);
	float tFarX = (std::max)(txMin, txMax);
	// Y軸
	float tyMin = (aabb.min_.y - line.origin.y) / line.diff.y;
	float tyMax = (aabb.max_.y - line.origin.y) / line.diff.y;
	float tNearY = ((std::min))(tyMin, tyMax);
	float tFarY = (std::max)(tyMin, tyMax);
	// Z軸
	float tzMin = (aabb.min_.z - line.origin.z) / line.diff.z;
	float tzMax = (aabb.max_.z - line.origin.z) / line.diff.z;
	float tNearZ = (std::min)(tzMin, tzMax);
	float tFarZ = (std::max)(tzMin, tzMax);
	// 各軸のtの最小値と最大値を計算する
	// AABBと衝突点（貫通点）のtが小さい方
	float tMin = (std::max)((std::max)(tNearX, tNearY), tNearZ);
	// AABBと衝突点（貫通点）のtが大きい方
	float tMax = (std::min)((std::min)(tFarX, tFarY), tFarZ);
	if (tMin > tMax) {
		return false;
	}
	return true;
}

bool IsCollision(const OBB& obb, const Sphere& sphere) {
	Matrix4x4 OBBWorldMatrix = OBBMakeWorldMatrix(obb);

	Vector3 centerInOBBLocalSpace = Transform(sphere.center_, Inverse(OBBWorldMatrix));

	AABB aabbOBBLocal{.min_ = -obb.size_, .max_ = obb.size_};
	Sphere sphereOBBLocal{centerInOBBLocalSpace, sphere.radius_};
	// ローカル空間で衝突判定
	if (IsCollision(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}
	return false;
}

bool IsCollision(const OBB& obb, const Segment& segment) {
	Matrix4x4 ObbWorldMatrix = OBBMakeWorldMatrix(obb);
	Matrix4x4 ObbInverse = Inverse(ObbWorldMatrix);

	Vector3 localOrigin = Transform(segment.origin, ObbInverse);
	Vector3 localEnd = Transform(segment.origin + segment.diff, ObbInverse);

	AABB localAABB{
	    {-obb.size_.x, -obb.size_.y, -obb.size_.z},
	    {+obb.size_.x, +obb.size_.y, +obb.size_.z},
	};

	Segment localSegment{
	    .origin{localOrigin},
	    .diff{localEnd - localOrigin},
	};
	// ローカル空間で当たり判定
	if (IsCollision(localAABB, localSegment)) {
		return true;
	}
	return false;
}

bool IsCollision(const OBB& obb, const Ray& ray) {
	Matrix4x4 ObbWorldMatrix = OBBMakeWorldMatrix(obb);
	Matrix4x4 ObbInverse = Inverse(ObbWorldMatrix);

	Vector3 localOrigin = Transform(ray.origin, ObbInverse);
	Vector3 localEnd = Transform(ray.origin + ray.diff, ObbInverse);

	AABB localAABB{
	    {-obb.size_.x, -obb.size_.y, -obb.size_.z},
	    {+obb.size_.x, +obb.size_.y, +obb.size_.z},
	};

	Ray localRay{
	    .origin{localOrigin},
	    .diff{localEnd - localOrigin},
	};
	// ローカル空間で当たり判定
	if (IsCollision(localAABB, localRay)) {
		return true;
	}
	return false;
}

bool IsCollision(const OBB& obb, const Line& line) {
	Matrix4x4 ObbWorldMatrix = OBBMakeWorldMatrix(obb);
	Matrix4x4 ObbInverse = Inverse(ObbWorldMatrix);

	Vector3 localOrigin = Transform(line.origin, ObbInverse);
	Vector3 localEnd = Transform(line.origin + line.diff, ObbInverse);

	AABB localAABB{
	    {-obb.size_.x, -obb.size_.y, -obb.size_.z},
	    {+obb.size_.x, +obb.size_.y, +obb.size_.z},
	};

	Line localLine{
	    .origin{localOrigin},
	    .diff{localEnd - localOrigin},
	};
	// ローカル空間で当たり判定
	if (IsCollision(localAABB, localLine)) {
		return true;
	}
	return false;
}

bool IsCollision(const OBB& obb_1, const OBB& obb_2) {
	const int32_t kAxis = 3;
	for (size_t i = 0; i < kAxis; i++) {
		// obb_1の軸
		if (SeparationAxis(obb_1.orientations_[i], obb_1, obb_2)) {
			return false;
		}
		// obb_2の軸
		if (SeparationAxis(obb_2.orientations_[i], obb_1, obb_2)) {
			return false;
		}
		for (size_t j = 0; j < kAxis; j++) {
			Vector3 tmp = Cross(obb_1.orientations_[i], obb_2.orientations_[j]);
			if (SeparationAxis(obb_1.orientations_[i], obb_1, obb_2)) {
				return false;
			}
		}
	}
	return true;
}
