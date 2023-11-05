#pragma once

#include <cstdint>
#include <sstream>

#include "Model.h"
#include "ViewProjection.h"

const static uint32_t kBlockSize = 2;
const static uint32_t kBlockScreenSize = 40;
const static uint32_t kMaxWidthBlockNum = 400;
const static uint32_t kMaxHeightBlockNum = 18;
const static uint32_t kMaxScreenWidthBlockNum = 32;
const static uint32_t kMaxScreenHeightBlockNum = kMaxHeightBlockNum;
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
	void Draw(const ViewProjection& viewProjection);

	uint32_t GetBlocksType(uint32_t x, uint32_t y) { return map_[y][x]; }
	uint32_t GetBlocksType(int x, int y) { return map_[static_cast<uint32_t>(y)][static_cast<uint32_t>(x)]; }
	uint32_t GetBlocksType(const Vector3& pos) { return (map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
	uint32_t GetBlocksType(const Vector2& pos) { return(map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
	Vector3 GetBlocksCenterWorldPosition(uint32_t x, uint32_t y);
	void SetBlocks(const Vector3& pos, uint32_t blockType);
	void SetBlocks(const Vector2& pos, uint32_t blockType);
	void SetViewProjection(ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	bool InRange(const Vector3& pos);
private:
	const uint32_t kMaxTypeBlocks = static_cast<uint32_t>(MapChip::Blocks::kCount);
	ViewProjection* viewProjection_;
	uint32_t map_[kMaxHeightBlockNum][kMaxWidthBlockNum];
	std::vector<Model*> blockModels_[kMaxHeightBlockNum][kMaxWidthBlockNum];
	WorldTransform blockWorldTransform_[kMaxHeightBlockNum][kMaxWidthBlockNum];
};

