#pragma once
#include "AABB.h"
#include "Line.h"
#include "OBB.h"
#include "Plane.h"
#include "Sphere.h"
#include "Triangle.h"
// 球と球
bool IsCollision(const Sphere& s1, const Sphere& s2);
// 球と平面
bool IsCollision(const Sphere& sphere, const Plane& plane);
// 線分と平面
bool IsCollision(const Plane& plane, const Segment& segment);
// 半直線と平面
bool IsCollision(const Plane& plane, const Ray& ray);
// 直線と平面
bool IsCollision(const Plane& plane, const Line& line);
// 三角形と線分
bool IsCollision(const Triangle& triangle, const Segment& segment);
// 三角形と半直線
bool IsCollision(const Triangle& triangle, const Ray& ray);
// 三角形と直線
bool IsCollision(const Triangle& triangle, const Line& line);
// AABBとAABB
bool IsCollision(const AABB& a, const AABB& b);
// AABBと球
bool IsCollision(const AABB& aabb, const Sphere& sphere);
// AABBと線分
bool IsCollision(const AABB& aabb, const Segment& segment);
// AABBと半直線
bool IsCollision(const AABB& aabb, const Ray& ray);
// AABBと直線
bool IsCollision(const AABB& aabb, const Line& line);
// OBBと球
bool IsCollision(const OBB& obb, const Sphere& sphere);
// OBBと線分
bool IsCollision(const OBB& obb, const Segment& segment);
// OBBと半直線
bool IsCollision(const OBB& obb, const Ray& ray);
// OBBと直線
bool IsCollision(const OBB& obb, const Line& line);
// OBBとOBB
bool IsCollision(const OBB& obb1, const OBB& obb2);