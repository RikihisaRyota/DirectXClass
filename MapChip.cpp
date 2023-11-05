#include "MapChip.h"

#include <cassert>
#include <fstream>
#include <string>

#include "ModelManager.h"
#include "MyMath.h"

void MapChip::Initialize() {
	stageName_ = {
		 "stage_1",
		 "stage_2",
		 "stage_3",
		 "stage_4",
		 "stage_5",
		 "stage_6",
		 "stage_7",
		 "stage_8"
	};

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
				);
				blockWorldTransform_[y][x].UpdateMatrix();
			}
		}
	}
}

void MapChip::LoadCSV() {
	LoadCSV(stageName_.at(currentStage_));
}

void MapChip::LoadCSV(uint32_t stageNum) {
	LoadCSV(stageName_.at(stageNum));
}

void MapChip::LoadCSV(std::string fileName) {
	// ファイル名をパスに追加
	fileName = "Resources/CSVs/" + fileName + ".csv";

	// 読み込むファイルを開く
	std::ifstream inputCSVFile(fileName);
	assert(inputCSVFile.is_open());

	// 行ごとに読み込み
	std::string line;
	uint32_t y = 0; // 行数
	while (getline(inputCSVFile, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);
		uint32_t x = 0; // 列数

		// mapに代入
		std::string cell;
		while (getline(lineStream, cell, ',')) {
			uint32_t value = std::stoi(cell);
			map_[y][x] = value;

			x++;
		}

		y++;
	}

	// ファイルを閉じる
	inputCSVFile.close();
}

void MapChip::SaveCSV() {
	SaveCSV(stageName_.at(currentStage_));
}

void MapChip::SaveCSV(uint32_t stageNum) {
	SaveCSV(stageName_.at(stageNum));
}

void MapChip::SaveCSV(std::string fileName) {
	// ファイル名をパスに追加
	fileName = "Resources/CSVs/" + fileName + ".csv";

	// 書き込むファイルを開く
	std::ofstream outputCSVFile(fileName);
	assert(outputCSVFile.is_open());

	// copyMap_ の内容をCSVファイルに書き込む
	for (int y = 0; y < kMaxHeightBlockNum; ++y) {
		for (int x = 0; x < kMaxWidthBlockNum; ++x) {
			// コンマ区切りでデータを書き込む
			outputCSVFile << map_[y][x];
			outputCSVFile << ",";
		}
		// 行末に改行を挿入
		outputCSVFile << std::endl;
	}

	// ファイルを閉じる
	outputCSVFile.close();
}

void MapChip::Draw(const ViewProjection& viewProjection) {
	int32_t xMin = int32_t(int32_t(viewProjection.translation_.x) / int32_t(kBlockSize)-int32_t(kMaxScreenWidthBlockNum) / 2)-1;
	if (xMin < 0) {
		xMin = 0;
	}
	int32_t xMax = int32_t(int32_t(viewProjection.translation_.x) / int32_t(kBlockSize) + int32_t(kMaxScreenWidthBlockNum) / 2)+1;
	if (xMax < 0) {
		xMax = 0;
	}
	for (int32_t y = 0; y < kMaxHeightBlockNum; y++) {
		for (int32_t x = xMin; x < xMax; x++) {
			auto blockType = map_[y][x];
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

Vector3 MapChip::GetBlocksCenterWorldPosition(uint32_t x, uint32_t y) {
	return MakeTranslate(blockWorldTransform_[y][x].matWorld_);
}

bool MapChip::InRange(const Vector3& pos) {
	if (pos.x < 0.0f || pos.x > float(kMaxWidth)) {
		return false;
	}
	if (pos.y < 0.0f || pos.y > float(kMaxHeight)) {
		return false;
	}
	return true;
}

void MapChip::SetBlocks(const Vector3& pos, uint32_t blockType) {
	int32_t x = int32_t(pos.x / float(kBlockSize));
	int32_t y = kMaxHeightBlockNum - int32_t(pos.y / float(kBlockSize));
	map_[y][x] = blockType;
}

void MapChip::SetBlocks(const Vector2& pos, uint32_t blockType) {
	uint32_t x = uint32_t(pos.x / kBlockScreenSize);
	// カメラの初期値
	float cameraPosX = 32.08f;
	uint32_t difference = uint32_t((viewProjection_->translation_.x - cameraPosX) / float(kBlockSize));
	uint32_t y = uint32_t(pos.y / kBlockScreenSize);
	map_[y][x+difference] = blockType;
}
