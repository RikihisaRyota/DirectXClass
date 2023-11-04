#include "MapChipEditor.h"

#include "Draw.h"
#include "MapChip.h"

void MapChipEditor::Initialize() {
	input_ = Input::GetInstance();
	isDebug_ = false;
	blockCount_ = 0;
	//mouse_.reset(SphereRenderer::Create());
	//mouseWorldTransform_.Initialize();
}

void MapChipEditor::Update() {
	if (input_->TriggerKey(DIK_TAB)) {
		isDebug_ ^= true;
	}
	if (isDebug_) {
#pragma region ブロック選択
		// ホイール
		if (input_->GetWheel() != 0) {

		}
		// アローキー
		if (input_->TriggerKey(DIK_UPARROW)) {
			blockCount_++;
			if (blockCount_ >= kMaxBlock_) {
				blockCount_ = 0;
			}
		}
		if (input_->TriggerKey(DIK_DOWNARROW)) {
			blockCount_--;
			if (blockCount_ >= 0) {
				blockCount_ = kMaxBlock_;
			}
		}
#pragma endregion
		Vector3 worldMousePos = input_->GetMouseWorldPosition(-viewProjection_->translation_.z, *viewProjection_);
		mouseWorldTransform_.translation_ = worldMousePos;
		mouseWorldTransform_.UpdateMatrix();
		// 左クリック中
		if (input_->PushMouse(0)) {
			// マウスのワールド座標を取得
			// マップチップ内でクリックしていたら
			if (mapChip_->InRange(worldMousePos)) {
				mapChip_->SetBlocks(worldMousePos, blockCount_);
				mapChip_->SaveCSV("stage_1");
			}
		}
	}
}

void MapChipEditor::Draw() {
	if (isDebug_) {
		//mouse_->Draw(mouseWorldTransform_,*viewProjection_);
		FrameDraw();
	}
}

void MapChipEditor::FrameDraw() {
	for (uint32_t y = 0; y < kMaxHeightBlockNum; y++) {
		DrawLine(
			Vector3(0.0f, float(y + kBlockSize), 0.0f),
			Vector3(float(kMaxWidth), float(y + kBlockSize), 0.0f),
			*viewProjection_,
			Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	for (uint32_t x = 0; x < kMaxWidthBlockNum; x++) {
		DrawLine(
			Vector3(float(x * kBlockSize), 0.0f, 0.0f),
			Vector3(float(x * kBlockSize), float(kMaxHeight), 0.0f),
			*viewProjection_,
			Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	}
}
