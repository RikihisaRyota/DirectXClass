#include "Vector3.h"

Vector3 Normalize(const Vector3& v) {
	Vector3 tmp = v;
	float length = sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	assert(length != 0.0f);
	tmp.x /= length;
	tmp.y /= length;
	tmp.z /= length;
	return tmp;
}

float Dot(const Vector3& a, const Vector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return { 
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x 
	};
}
