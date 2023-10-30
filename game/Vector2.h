#pragma once
#include <cmath>

struct Vector2 {
	float x;
	float y;

	// ベクトルの長さを計算する関数
	float Length() const { return std::sqrt(x * x + y * y); }

	// ベクトルの正規化を行う関数
	void Normalize() {
		float length = Length();
		x /= length;
		y /= length;
	}
	// 加算演算子のオーバーロード
	Vector2 operator+(const Vector2& other) const {
		return { x + other.x, y + other.y };
	}
	// 加算演算子のオーバーロード
	Vector2 operator+(float other) const {
		return { x + other, y + other};
	}

	// 減算演算子のオーバーロード
	Vector2 operator-(const Vector2& other) const {
		return { x - other.x, y - other.y};
	}
	// 減算演算子のオーバーロード
	Vector2 operator-(float other) const {
		return { x - other, y - other};
	}

	// 二項マイナス演算子のオーバーロード（引数なしの単項マイナス演算子ではない）
	Vector2 operator-() const { return { -x, -y}; }

	// 積演算子のオーバーロード
	Vector2 operator*(const Vector2& other) const {
		return { x * other.x, y * other.y};
	}

	// スカラー乗算演算子のオーバーロード
	Vector2 operator*(float scalar) const { return { x * scalar, y * scalar}; }

	// スカラー除算演算子のオーバーロード
	Vector2 operator/(float scalar) const { return { x / scalar, y / scalar}; }

	// +=演算子のオーバーロード
	Vector2& operator+=(const Vector2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	// -=演算子のオーバーロード
	Vector2& operator-=(const Vector2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	// *=演算子のオーバーロード
	Vector2& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	// /=演算子のオーバーロード
	Vector2& operator/=(float scalar) {
		x /= scalar;
		y /= scalar;
		return *this;
	}

	// !=演算子のオーバーロード
	bool operator!=(const Vector2& other) const {
		return !(x == other.x && y == other.y);
	}

	// ==演算子のオーバーロード
	bool operator==(const Vector2& other) const {
		return (x == other.x && y == other.y);
	}

	Vector2 operator-() { return Vector2(-x, -y); }
};