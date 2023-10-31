#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// 頂点データ構造体
/// </summary>
struct cVertexPos {
	Vector4 pos_; // xyz座標
	Vector3 normal_; // 法線
	Vector2 texcoord_; // uv座標
};