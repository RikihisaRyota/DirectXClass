#pragma once

#include <assert.h>
#include <cstdint>
#include <map>
#include <string>
#include <variant>

#include "Vector3.h"

class GlobalVariables {
public:
	// 項目
	using Item = std::variant<int32_t, float, Vector3>;
	// グループ
	using Group = std::map<std::string, Item>;
	// 全データ
	std::map<std::string, Group> datas_;

public:
	static GlobalVariables* GetInstance();
	// インスタンスを複製しないためのあれこれ
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	const GlobalVariables& operator=(const GlobalVariables&) = delete;
	/// <summary>
	/// グループの作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);
	void Update();
	/// <summary>
	/// ファイルに書き出し
	/// </summary>
	/// <param name="groupName">グループ</param>
	void SaveFile(const std::string& groupName);
	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();
	/// <summary>
	/// ディレクトリのファイル読み込み
	/// </summary>
	void LoadFile(const std::string& groupName);
	
public:
	// 値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	// 値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	// 値のセット
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);
	template<class T> T GetValue(const std::string& groupName, const std::string& key) const;
	//// 値のゲット(int)
	//int32_t GetValue(const std::string& groupName, const std::string& key) const;
	//// 値のゲット(float)
	//float GetValue(const std::string& groupName, const std::string& key) const;
	//// 値のゲット(Vector3)
	//Vector3 GetValue(const std::string& groupName, const std::string& key) const;
	// 項目の追加(int)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	// 項目の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	// 項目の追加(vector3)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

private:
	int32_t save_int_Value_;
	float save_float_Value_;
};

template<class T>
inline T GlobalVariables::GetValue(const std::string& groupName, const std::string& key) const {
	// グループを検索
	const auto& groupIt = datas_.find(groupName);
	// 未登録をチェック
	assert(groupIt != datas_.end());

	const Group& group = groupIt->second;
	// 指定キーが存在するか
	assert(group.count(key) != 0);

	const Item& item = group.at(key);
	return std::get<T>(item);
}
