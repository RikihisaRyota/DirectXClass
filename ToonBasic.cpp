#include "ToonBasic.h"

#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"

ToonBasic* ToonBasic::Create() {
	// ToonBasicのインスタンスを生成
	ToonBasic* basic = new ToonBasic();
	assert(basic);

	// 初期化
	basic->outlineRenderer_ = Basic::Create(false);
	basic->originalRenderer_ = Basic::Create(true);

	return basic;
}

void ToonBasic::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	
	originalRenderer_->Draw(worldTransform, viewProjection, textureHadle);
	outlineRenderer_->Draw(worldTransform, viewProjection, textureHadle);
}
