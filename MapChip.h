#pragma once

#include <cstdint>
#include <sstream>

#include "Model.h"
#include "ViewProjection.h"

const static uint32_t kBlockSize = 2;
const static uint32_t kMaxWidthBlockNum = 400;
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
	void LoadCSV(std::string fileName);
	void SaveCSV(std::string fileName);

	uint32_t GetBlocksType(uint32_t x, uint32_t y) { return map_[y][x]; }
	uint32_t GetBlocksType(int x, int y) { return map_[static_cast<uint32_t>(y)][static_cast<uint32_t>(x)]; }
	uint32_t GetBlocksType(const Vector3& pos) { return (map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
	uint32_t GetBlocksType(const Vector2& pos) { return(map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
	Vector3 GetBlocksCenterWorldPosition(uint32_t x, uint32_t y) { return Vector3(static_cast<float>(kBlockSize * x) + static_cast<float>(kBlockSize) * 0.5f, static_cast<float>(kBlockSize * y) + static_cast<float>(kBlockSize) * 0.5f, 0.0f); }
	bool InRange(const Vector3& pos);
	void SetBlocks(const Vector3& pos, uint32_t blockType);
private:
	const uint32_t kMaxTypeBlocks = static_cast<uint32_t>(MapChip::Blocks::kCount);
	uint32_t map_[kMaxHeightBlockNum][kMaxWidthBlockNum];
};

