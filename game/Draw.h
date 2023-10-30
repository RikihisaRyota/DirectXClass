#include <cstdint>

struct AABB;
struct OBB;
struct Plane;
struct Sphere;
struct Triangle;
struct ViewProjection;
struct Vector3;
struct Vector4;

void DrawLine(
    const Vector3& v1, const Vector3& v2, const ViewProjection& viewProjection, Vector4 color);

void DrawGrid(const ViewProjection& viewProjection);

void DrawSphere(const Sphere& sphere, const ViewProjection& viewProjection,Vector4 color);

void DrawPlane(const Plane& plane, const ViewProjection& viewProjection, Vector4 color);

void DrawTriangle(const Triangle& triangle, const ViewProjection& viewProjection,Vector4 color);

void DrawAABB(const AABB& aabb, const ViewProjection& viewProjection,Vector4 color);

void DrawOBB(const OBB& obb, const ViewProjection& viewProjection,Vector4 color);

void DrawBezier(const Vector3& v1, const Vector3& v2, const Vector3& v3, const ViewProjection& viewProjection,Vector4 color);

void DrawCatmullRom(const Vector3 v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, const ViewProjection& viewProjection, Vector4 color);