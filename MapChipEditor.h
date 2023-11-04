#pragma once

#include "Input.h"
#include "ViewProjection.h"

class MapChip;
class MapChipEditor {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void SetMapChip(MapChip* mapChip) { mapChip_ = mapChip; }
private:
	void FrameDraw(const ViewProjection& viewProjection);
	MapChip* mapChip_;
	Input* input_;
	bool isDebug_;
};

