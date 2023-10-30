#pragma once

#include "Vector4.h"
#include "Vector3.h"

/// <summary>
/// ライト
/// </summary>
struct cDirectionalLight {
	Vector4 color_; //!< ライトの色
	Vector3 direction_; //!< ライトの位置
	float intensiy_; //!< 輝度
	float sharpness_; //<! スペキュラーの反射度
};