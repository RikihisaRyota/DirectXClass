#include "Draw.h"

#include "AABB.h"
#include "MyMath.h"
#include "OBB.h"
#include "Plane.h"
#include "PrimitiveDrawer.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Vector3.h"
#include "Vector4.h"
#include "ViewProjection.h"

void DrawLine(
    const Vector3& v1, const Vector3& v2, const ViewProjection& viewProjection, Vector4 color) {
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);
	Vector3 ndcVertex_Start = Transform(v1, viewProjection.matView);
	Vector3 screenVertices_Start = Transform(ndcVertex_Start, viewProjection.matProjection);

	Vector3 ndcVertex_End = Transform(v2, viewProjection.matView);
	Vector3 screenVertices_End = Transform(ndcVertex_End, viewProjection.matProjection);
	PrimitiveDrawer::GetInstance()->DrawLine3d(v1, v2, color);
}

void DrawGrid(const ViewProjection& viewProjection) {
	const float kGridHalfWidth = 2.0f;                                      // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                       // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 一つ分の長さ
	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		// グリットのスケール
		Vector3 scale(1.0f, 1.0f, 1.0f);
		// グリットの回転
		Vector3 rotate(0.0f, 0.0f, 0.0f);
		// 始点をずらす
		int i = xIndex - kSubdivision / 2;

		Vector3 position_Start(-kGridHalfWidth, 0.0f, static_cast<float>(kGridEvery * i));
		Vector3 position_End(kGridHalfWidth, 0.0f, static_cast<float>(kGridEvery * i));

		if (xIndex == kSubdivision / 2) {
			DrawLine(position_Start, position_End, viewProjection, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		} else {
			DrawLine(position_Start, position_End, viewProjection, Vector4(0.5f, 0.5f, 0.5f, 1.0f));
		}
	}
	// 左から右
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		Vector3 scale(1.0f, 1.0f, 1.0f);
		Vector3 rotate(0.0f, 0.0f, 0.0f);
		int i = zIndex - kSubdivision / 2;
		Vector3 position_Start(static_cast<float>(kGridEvery * i), 0.0f, -kGridHalfWidth);
		Vector3 position_End(static_cast<float>(kGridEvery * i), 0.0f, kGridHalfWidth);

		if (zIndex == kSubdivision / 2) {
			DrawLine(position_Start, position_End, viewProjection, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		} else {
			DrawLine(position_Start, position_End, viewProjection, Vector4(0.5f, 0.5f, 0.5f, 1.0f));
		}
	}
}

void DrawSphere(const Sphere& sphere, const ViewProjection& viewProjection, Vector4 color) {
	double M_PI = 3.141592653589793;
	// 分割数
	const uint32_t kSubdivision = 16;
	// 経度分割1つ分の角度
	const float kLonEvery = static_cast<float>(M_PI) * 2.0f / kSubdivision;
	;
	// 緯度分割1つ分の角度
	const float kLatEvery = static_cast<float>(M_PI) / kSubdivision;

	// 緯度の方向に分割 -M_PI/2 ~ M_PI/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		// 現在の緯度
		float lat = static_cast<float>(-M_PI) / 2.0f + kLatEvery * latIndex;
		// 経度の方向に分割 0 ~ 2*M_PI
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			// 現在の経度
			float lon = lonIndex * kLonEvery;
			// world座標系でのa,b,cを求める
			Vector3 a, b, c;
			a = sphere.center_ + (Vector3(std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon)) * sphere.radius_);
			b = sphere.center_ + (Vector3(std::cos(lat + kLatEvery) * std::cos(lon), std::sin(lat + kLatEvery),std::cos(lat + kLatEvery) * std::sin(lon)) * sphere.radius_);
			c = sphere.center_ + (Vector3(std::cos(lat) * std::cos(lon + kLonEvery), std::sin(lat) , std::cos(lat) * std::sin(lon + kLonEvery)) * sphere.radius_);

			DrawLine(a, b, viewProjection, color);
			DrawLine(a, c, viewProjection, color);
		}
	}
}

void DrawPlane(const Plane& plane, const ViewProjection& viewProjection, Vector4 color) {
	Vector3 center = plane.normal_ * plane.distanse_; // 1
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal_)); // 2
	perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z};
	perpendiculars[2] = Cross(plane.normal_, perpendiculars[0]); // 4
	perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z};
	// 6
	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = perpendiculars[index] * 2.0f;
		Vector3 point = center + extend;
		points[index] = point;
	}
	DrawLine(points[0], points[3], viewProjection ,color);
	DrawLine(points[0], points[2], viewProjection ,color);
	DrawLine(points[1], points[2], viewProjection ,color);
	DrawLine(points[1], points[3], viewProjection ,color);
}

void DrawTriangle(const Triangle& triangle, const ViewProjection& viewProjection, Vector4 color) {
	DrawLine(triangle.vertices_[0], triangle.vertices_[1], viewProjection,color); // 1から2
	DrawLine(triangle.vertices_[1], triangle.vertices_[2], viewProjection,color); // 2から3
	DrawLine(triangle.vertices_[2], triangle.vertices_[0], viewProjection, color); // 3から1
}

void DrawAABB(const AABB& aabb, const ViewProjection& viewProjection, Vector4 color) {
	Vector3 vertices[] = {
	    {aabb.min_},
	    {aabb.max_.x, aabb.min_.y, aabb.min_.z},
	    {aabb.max_.x, aabb.min_.y, aabb.max_.z},
	    {aabb.min_.x, aabb.min_.y, aabb.max_.z},
	    {aabb.min_.x, aabb.max_.y, aabb.min_.z},
	    {aabb.max_.x, aabb.max_.y, aabb.min_.z},
	    {aabb.max_},
	    {aabb.min_.x, aabb.max_.y, aabb.max_.z},
	};
	for (int i = 0; i < 4; i++) {
		int j = (i + 1) % 4;
		DrawLine(vertices[i], vertices[j], viewProjection, color);
		DrawLine(vertices[i], vertices[i + 4], viewProjection, color);
		DrawLine(vertices[i + 4], vertices[j + 4], viewProjection, color);
	}
}

void DrawOBB(const OBB& obb, const ViewProjection& viewProjection, Vector4 color) {
	Vector3 vertices[] = {
	    {-obb.size_},
	    {+obb.size_.x, -obb.size_.y, -obb.size_.z},
	    {+obb.size_.x, -obb.size_.y, +obb.size_.z},
	    {-obb.size_.x, -obb.size_.y, +obb.size_.z},
	    {-obb.size_.x, +obb.size_.y, -obb.size_.z},
	    {+obb.size_.x, +obb.size_.y, -obb.size_.z},
	    {obb.size_},
	    {-obb.size_.x, +obb.size_.y, +obb.size_.z},
	};

	Matrix4x4 rotateMatrix = MakeIdentity4x4();
	rotateMatrix = SetRotate(obb.orientations_);
	for (auto& vertex : vertices) {
		vertex = Transform(vertex, rotateMatrix);
		vertex = vertex + obb.center_;
	}

	for (int i = 0; i < 4; i++) {
		int j = (i + 1) % 4;
		DrawLine(vertices[i], vertices[j], viewProjection,color);
		DrawLine(vertices[i], vertices[i + 4], viewProjection, color);
		DrawLine(vertices[i + 4], vertices[j + 4], viewProjection, color);
	}
}

void DrawBezier(const Vector3& v1, const Vector3& v2, const Vector3& v3, const ViewProjection& viewProjection,Vector4 color) {
	const int32_t DivisionCount = 16;
	for (float i = 1.0f; i < DivisionCount; i++) {
		DrawLine(
		    CubicBezier(v1, v2, v3, static_cast<float>(i - 1) / static_cast<float>(DivisionCount)),
		    CubicBezier(v1, v2, v3, static_cast<float>(i) / static_cast<float>(DivisionCount)),
		    viewProjection, color);
	}
}

void DrawCatmullRom(const Vector3 v0, const Vector3& v1, const Vector3& v2, const Vector3& v3,const ViewProjection& viewProjection, Vector4 color) {
	const int32_t DivisionCount = 100;
	for (float i = 1.0f; i < DivisionCount; i++) {
		// p0,p1間
		DrawLine(
		    CubicCatmullRom(
		        v0, v0, v1, v2, static_cast<float>(i - 1) / static_cast<float>(DivisionCount)),
		    CubicCatmullRom(
		        v0, v0, v1, v2, static_cast<float>(i) / static_cast<float>(DivisionCount)),
		    viewProjection, color);
		// p1,p2間
		DrawLine(
		    CubicCatmullRom(
		        v0, v1, v2, v3, static_cast<float>(i - 1) / static_cast<float>(DivisionCount)),
		    CubicCatmullRom(
		        v0, v1, v2, v3, static_cast<float>(i) / static_cast<float>(DivisionCount)),
		    viewProjection, color);
		// p2,p3間
		DrawLine(
		    CubicCatmullRom(
		        v1, v2, v3, v3, static_cast<float>(i - 1) / static_cast<float>(DivisionCount)),
		    CubicCatmullRom(
		        v1, v2, v3, v3, static_cast<float>(i) / static_cast<float>(DivisionCount)),
		    viewProjection, color);
	}
}
