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
	void Initialize();
	void Draw(const ViewProjection& viewProjection);

#pragma region Load,Save
	void LoadCSV();
	void LoadCSV(uint32_t stageNum);
	void LoadCSV(std::string fileName);
	void SaveCSV();
	void SaveCSV(uint32_t stageNum);
	void SaveCSV(std::string fileName);
#pragma endregion
#pragma region BlockType
	uint32_t GetBlocksType(uint32_t x, uint32_t y) { return map_[y][x]; }
	uint32_t GetBlocksType(int x, int y) { return map_[static_cast<uint32_t>(y)][static_cast<uint32_t>(x)]; }
	uint32_t GetBlocksType(const Vector3& pos) { return (map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
	uint32_t GetBlocksType(const Vector2& pos) { return(map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
#pragma endregion
	Vector3 GetBlocksCenterWorldPosition(uint32_t x, uint32_t y);

	uint32_t GetCurrentStage() { return currentStage_; }
	void SetCurrentStage(uint32_t stageNum) { currentStage_ = stageNum; }
	void SetBlocks(const Vector3& pos, uint32_t blockType);
	void SetBlocks(const Vector2& pos, uint32_t blockType);
	void SetViewProjection(ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	bool InRange(const Vector3& pos);
private:
	// ブロックの種類の最大数
	const uint32_t kMaxTypeBlocks = static_cast<uint32_t>(MapChip::Blocks::kCount);
	ViewProjection* viewProjection_;
	// マップチップの種類
	uint32_t map_[kMaxHeightBlockNum][kMaxWidthBlockNum];
	// ブロックのモデル
	std::vector<Model*> blockModels_[kMaxHeightBlockNum][kMaxWidthBlockNum];
	// ブロックのワールドトランスフォーム
	WorldTransform blockWorldTransform_[kMaxHeightBlockNum][kMaxWidthBlockNum];
	// CSVの名前保存
	std::vector<std::string> stageName_;
	// 現在のステージ
	uint32_t currentStage_;
};

