#pragma once
#include <memory>

#include "Model.h"
#include "WorldTransform.h"
class Skydome {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::unique_ptr<Model> model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);
	
	float scale_;
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	std::unique_ptr<Model> model_ = nullptr;
};
