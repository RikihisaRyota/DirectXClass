#pragma once

#include <memory>

#include "Input.h"
#include "MapChip.h"
#include "ViewProjection.h"
#include "SphereRenderer.h"

class MapChipEditor {
public:
	void Initialize();
	void Update();
	void Draw();

	void SetViewProjection(ViewProjection* ViewProjection) { viewProjection_ = ViewProjection; }
	void SetMapChip(MapChip* mapChip) { mapChip_ = mapChip; }
private:
	void FrameDraw();
	const uint32_t kMaxBlock_= uint32_t(MapChip::Blocks::kCount);
	ViewProjection* viewProjection_;
	MapChip* mapChip_;
	Input* input_;
	bool isDebug_;
	uint32_t blockCount_;

	std::unique_ptr< SphereRenderer> mouse_;
	WorldTransform mouseWorldTransform_;
};

