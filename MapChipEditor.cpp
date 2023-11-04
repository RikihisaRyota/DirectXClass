#include "MapChipEditor.h"

#include "Draw.h"
#include "MapChip.h"

void MapChipEditor::Initialize() {
	input_ = Input::GetInstance();
	isDebug_ = false;
}

void MapChipEditor::Update() {}

void MapChipEditor::Draw(const ViewProjection& viewProjection) {
}

void MapChipEditor::FrameDraw(const ViewProjection& viewProjection) {
	for (uint32_t y = 0; y < kMaxHeightBlockNum; y++) {
		for (uint32_t x = 0; x < kMaxWidthBlockNum;x++) {
			
		}
	}
}
