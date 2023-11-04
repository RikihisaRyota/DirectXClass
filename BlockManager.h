#pragma once

#include <cstdint>
#include <vector>
#include <map>

#include "MapChip.h"
#include "Model.h"
#include "WorldTransform.h"

class BlockManager {
public:
	void Initialize();
	void Draw(const ViewProjection& viewProjection);

	void SetMapChip(MapChip* mapChip) { mapChip_ = mapChip; }
private:
	MapChip* mapChip_;
	std::vector<Model*> blockModels_[kMaxHeightBlockNum][kMaxWidthBlockNum];
	WorldTransform blockWorldTransform_[kMaxHeightBlockNum][kMaxWidthBlockNum];
};

