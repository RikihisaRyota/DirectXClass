#pragma once

#include <memory>
#include <vector>

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
	int32_t blockCount_;
};

