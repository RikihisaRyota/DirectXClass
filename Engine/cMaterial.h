#pragma once

#include <cstdint>

#include "Vector4.h"
#include "Matrix4x4.h"

/// <summary>
/// マテリアル
/// </summary>
struct cMaterial {
	Vector4 color_; // 色
	uint32_t enableLightint_; //!< ライティングをするかどうか
	float padding[3];
	Matrix4x4 uvTransform_;
};