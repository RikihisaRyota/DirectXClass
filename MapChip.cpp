#include "MapChip.h"

#include <cassert>
#include <fstream>
#include <string>


MapChip::MapChip() {}

MapChip::~MapChip() {}

void MapChip::Initialize() {}

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
	uint32_t x = uint32_t(pos.x / float(kBlockSize));
	uint32_t y = kMaxHeightBlockNum - uint32_t(pos.y / float(kBlockSize));
	map_[y][x] = blockType;
}
