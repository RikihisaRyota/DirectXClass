#pragma once
#include <assert.h>
#include <cmath>

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3& operator+=(const Vector3& a) {
		this->x += a.x;
		this->y += a.y;
		this->z += a.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& a) {
		this->x -= a.x;
		this->y -= a.y;
		this->z -= a.z;
		return *this;
	}

	Vector3& operator*=(float a) {
		this->x *= a;
		this->y *= a;
		this->z *= a;
		return *this;
	}

	Vector3 operator+(const Vector3& a) const {
		return Vector3(this->x + a.x, this->y + a.y, this->z + a.z);
	}

	Vector3 operator-(const Vector3& a) const {
		return Vector3(this->x - a.x, this->y - a.y, this->z - a.z);
	}

	Vector3 operator*(const Vector3& a) const {
		return Vector3(this->x * a.x, this->y * a.y, this->z * a.z);
	}

	Vector3 operator*(const float a) const {
		return Vector3(this->x * a, this->y * a, this->z * a);
	}

	Vector3 operator+(const float a) const {
		return Vector3(this->x + a, this->y + a, this->z + a);
	}
	void Normalize() {
		float length = sqrt(x * x + y * y + z * z);
		assert(length != 0.0f);
		x /= length;
		y /= length;
		z /= length;
	}

	float length(Vector3 v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
};
// 正規化
Vector3 Normalize(const Vector3& v);
// 内積
float Dot(const Vector3& a, const Vector3& b);
//外積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
