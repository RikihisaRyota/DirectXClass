#pragma once

#include <cstdint>
#include <sstream>

#include "Model.h"
#include "ViewProjection.h"

const static uint32_t kBlockSize = 2;
const static uint32_t kMaxWidthBlockNum = 26;
const static uint32_t kMaxHeightBlockNum = 20;
const static uint32_t kMaxWidth = kBlockSize * kMaxWidthBlockNum;
const static uint32_t kMaxHeight = kBlockSize * kMaxHeightBlockNum;

class MapChip {
public:
	enum class Blocks {
		kNone,
		kBlock,
		kRedBlock,

		kCount,
	};
public:
	MapChip();
	~MapChip();
	void Initialize();
	void Draw(const ViewProjection& viewProjection);
	void LoadCSV(std::string fileName);

	uint32_t GetBlocksType(uint32_t x, uint32_t y) { return map_[y][x]; }
	uint32_t GetBlocksType(int x, int y) { return static_cast<int>(map_[static_cast<uint32_t>(y)][static_cast<uint32_t>(x)]); }
	Vector3 GetBlocksCenterWorldPosition(uint32_t x, uint32_t y) { return Vector3(static_cast<float>(kBlockSize * x) + static_cast<float>(kBlockSize) * 0.5f, static_cast<float>(kBlockSize * y) + static_cast<float>(kBlockSize) * 0.5f, 0.0f); }
private:
	const uint32_t kMaxTypeBlocks = static_cast<uint32_t>(MapChip::Blocks::kCount);
	uint32_t map_[kMaxHeightBlockNum][kMaxWidthBlockNum];
};

