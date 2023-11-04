#include "BlockManager.h"

#include "ModelManager.h"

void BlockManager::Initialize() {
	auto modelManager = ModelManager::GetInstance();
	for (uint32_t y = 0; y < kMaxHeightBlockNum; y++) {
		for (uint32_t x = 0; x < kMaxWidthBlockNum; x++) {
			for (size_t models = 0; models < modelManager->GetBlockModelSize(); models++) {
				blockModels_[y][x].emplace_back(modelManager->GetBlockModel(models));
				blockWorldTransform_[y][x].Initialize();
				blockWorldTransform_[y][x].translation_ = Vector3(
					float(x * kBlockSize) + float(kBlockSize) * 0.5f,
					float((kMaxHeightBlockNum - y) * kBlockSize) + float(kBlockSize) * 0.5f,
					0.0f
				);;
				blockWorldTransform_[y][x].UpdateMatrix();
			}
		}
	}
}

void BlockManager::Draw(const ViewProjection& viewProjection) {
	for (uint32_t y = 0; y < kMaxHeightBlockNum; y++) {
		for (uint32_t x = 0; x < kMaxWidthBlockNum; x++) {
			auto blockType = mapChip_->GetBlocksType(x, y);
			switch (blockType) {
			case static_cast<size_t>(MapChip::Blocks::kBlock):
			{
				blockModels_[y][x].at(static_cast<size_t>(MapChip::Blocks::kBlock) - 1)->Draw(blockWorldTransform_[y][x], viewProjection);
			}
			break;
			case static_cast<size_t>(MapChip::Blocks::kRedBlock):
			{
				blockModels_[y][x].at(static_cast<size_t>(MapChip::Blocks::kRedBlock) - 1)->Draw(blockWorldTransform_[y][x], viewProjection);
			}
			break;
			case static_cast<size_t>(MapChip::Blocks::kNone):
			{

			}
			break;
			default:
				break;
			}

		}
	}
}
