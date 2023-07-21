#pragma once

#include <cstdint>

#include "Vector4.h"

/// <summary>
/// マテリアル
/// </summary>
struct Material {
	Vector4 color_; // 色
	int32_t enableLightint_; //!< ライティングをするかどうか
};