#include "MapChip.h"

#include <cassert>
#include <fstream>
#include <string>


MapChip::MapChip() {}

MapChip::~MapChip() {}

void MapChip::Initialize() {}

void MapChip::Draw(const ViewProjection& viewProjection) {
    for (uint32_t y = 0; y < kMaxHeight; y++) {
        for (uint32_t x = 0; x < kMaxWidth; x++) {

        }
    }
}

void MapChip::LoadCSV(std::string fileName) {
    // ファイル名をパスに追加
    fileName = "Resources/CSV/" + fileName + ".csv";

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