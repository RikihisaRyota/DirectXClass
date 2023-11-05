#include "MapChipEditor.h"

#include "Draw.h"
#include "ImGuiManager.h"
#include "MapChip.h"
#include "ModelManager.h"
#include "TextureManager.h"


void MapChipEditor::Initialize() {
	input_ = Input::GetInstance();
	isDebug_ = false;
	blockCount_ = 0;
}

void MapChipEditor::Update() {
	if (input_->TriggerKey(DIK_TAB)) {
		isDebug_ ^= true;
	}
	if (isDebug_) {
#pragma region カメラ移動
		if (input_->PushKey(DIK_D)) {
			viewProjection_->translation_.x += 2.0f;
		}
		if (input_->PushKey(DIK_A)) {
			viewProjection_->translation_.x -= 2.0f;
		}
		viewProjection_->UpdateMatrix();
#pragma endregion
#pragma region ブロック選択
		// ホイール
		if (input_->GetWheel() != 0) {
			if (input_->GetWheel() < 0) {
				blockCount_--;
				if (blockCount_ < 0) {
					blockCount_ = kMaxBlock_ - 1;
				}
			}
			if (input_->GetWheel() > 0) {
				blockCount_++;
				if (blockCount_ >= int32_t(kMaxBlock_)) {
					blockCount_ = 0;
				}
			}
		}
		// アローキー
		if (input_->TriggerKey(DIK_DOWNARROW)) {
			blockCount_--;
			if (blockCount_ < 0) {
				blockCount_ = kMaxBlock_ - 1;
			}
		}
		if (input_->TriggerKey(DIK_UPARROW)) {
			blockCount_++;
			if (blockCount_ >= int32_t(kMaxBlock_)) {
				blockCount_ = 0;
			}
		}
#pragma endregion
		// 左クリック中
		if (input_->PushMouse(0)) {
			// マウスのワールド座標を取得
			Vector2 screenMousePos = input_->GetMouseScreenPosition();
			// マップチップ内でクリックしていたら
			if (screenMousePos.x >= 0.0f &&
				screenMousePos.x <= 1280.0f &&
				screenMousePos.y >= 0.0f &&
				screenMousePos.y <= 720.0f) {
				mapChip_->SetBlocks(screenMousePos, blockCount_);
				mapChip_->SaveCSV("stage_1");
			}
		}
#ifdef _DEBUG
		ImGui::ShowDemoWindow();
		ImGui::Begin("Debug");
		if (ImGui::TreeNode("MapChipEditor")) {
			ImGui::Text("block:%d", blockCount_);
			ImGui::BeginCombo();

			ImGui::TreePop();
		}
		ImGui::End();
#endif // _DEBUG

	}
}

void MapChipEditor::Draw() {
	if (isDebug_) {
		//FrameDraw();
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
