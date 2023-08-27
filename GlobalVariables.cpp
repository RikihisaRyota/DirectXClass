#include "GlobalVariables.h"

#include <json.hpp>
#include <fstream>

#include "ImGuiManager.h"

using namespace nlohmann;

GlobalVariables* GlobalVariables::GetInstance() { 
	static GlobalVariables instans;
	return &instans;
}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	// 指定名のオブジェクトが無ければ追加する
	datas_[groupName];
}

void GlobalVariables::Update() { 
#ifdef DEBUG
	// メニューバーを使用可能なフラグをつけてウィンドウを開く
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}
	// 各グループについて
	for (auto itGroup = datas_.begin(); itGroup != datas_.end(); ++itGroup) {
		// グループ名を取得
		const std::string& groupName = itGroup->first;
		// グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}
		// 各項目について
		for (auto itItem = group.begin(); itItem != group.end(); ++itItem) {
			// 項目名を所得
			const std::string& itemName = itItem->first;
			// 項目の参照を取得
			Item& item = itItem->second;
			// int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(
				    itemName.c_str(), ptr, save_int_Value_ - 100, save_int_Value_ + 100);
			} else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::SliderFloat(
				    itemName.c_str(), ptr, save_float_Value_ - 100.0f, save_float_Value_ + 100.0f);
			} else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);
			}
		}
		// 改行
		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json save.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
	ImGui::End();
#endif // DEBUG
}

void GlobalVariables::SaveFile(const std::string& groupName) {
#pragma region 全データをまとめる
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 未登録をチェック
	assert(itGroup != datas_.end());
	// jsonオブジェクト
	json root;
	root = json::object();
	// jsonオブジェクト登録
	root[groupName] = json::object();
	// 各項目について
	for (auto itItem = itGroup->second.begin();
		itItem != itGroup->second.end();
	     ++itItem) {
		// 項目名を取得
		const std::string& itemName = itItem->first;
		Item& item = itItem->second;
		// int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item)) {
			// int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item);
		} else if (std::holds_alternative<float>(item)) {
			// float型の値を登録
			root[groupName][itemName] = std::get<float>(item);
		} else if (std::holds_alternative<Vector3>(item)) {
			// float型のjson配列登録
			Vector3 value = std::get<Vector3>(item);
			root[groupName][itemName] = json::array({value.x, value.y, value.z});
		}
	}
#pragma endregion
#pragma region ファイルに書き出す
	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
	// ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}
	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 書き込み用ファイルストリーミング
	std::ofstream ofs;
	// ファイル書き込み用に開く
	ofs.open(filePath);
	// ファイルオープン失敗?
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
#ifdef DEBUG
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
#endif // DEBUG
		assert(0);
		return;
	}
	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	// ファイルを閉じる
	ofs.close();
#pragma endregion

	

}

void GlobalVariables::LoadFiles() {
	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
	// ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}
	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		// ファイルパスの取得
		const std::filesystem::path& filePath = entry.path();
		// ファイルの拡張子を取得
		std::string extension = filePath.extension().string();
		// .jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}
		// ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
	// 読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗?
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
#ifdef DEBUG
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
#endif // DEBUG
		assert(0);
		return;
	}
	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// グループを検索
	json::iterator itGroup = root.find(groupName);
	// 未登録チェック
	assert(itGroup != root.end());
	// 各アイテムについて
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		// アイテム名を取得
		const std::string& itemName = itItem.key();
		// int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			// int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		} else if (itItem->is_number_float()) {
			// float型に値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		} else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列登録
			Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
			SetValue(groupName, itemName, value);
		}
	}
}

void GlobalVariables::SetValue(
    const std::string& groupName, 
	const std::string& key, 
	int32_t value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	save_int_Value_ = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void GlobalVariables::SetValue(
	const std::string& groupName, 
	const std::string& key, 
	float value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	save_float_Value_ = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void GlobalVariables::SetValue(
    const std::string& groupName,
	const std::string& key, 
	const Vector3& value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void GlobalVariables::AddItem(
	const std::string& groupName,
	const std::string& key,
	int32_t value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 項目が未登録なら
	if (itGroup == datas_.end()) {
		SetValue(groupName,key,value);
	}
}

void GlobalVariables::AddItem(
	const std::string& groupName,
	const std::string& key,
	float value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 項目が未登録なら
	if (itGroup == datas_.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(
    const std::string& groupName,
	const std::string& key,
	const Vector3& value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 項目が未登録なら
	if (itGroup != datas_.end()) {
		SetValue(groupName, key, value);
	}
}
