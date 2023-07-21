#pragma once

#include <d3d12.h>
#include <vector>
#include <wrl.h>

#include "Vector3.h"
#include "Vector2.h"

class VertexBuffer {
public: // サブクラス
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexPos {
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線
		Vector2 texcoord; // uv座標
	};
	/// <sum
public:
	/// <summary>
	/// 頂点バッファ生成
	/// </summary>
	void Create(ID3D12Device* device, std::vector<VertexPos> vertices);
	/// <summary>
	/// 書き込む
	/// </summary>
	void UpLoadData();
public:
	/// <summary>
	/// view
	/// </summary>
	/// <returns></returns>
	D3D12_VERTEX_BUFFER_VIEW GetView() const {return vbView_;}
private:
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<VertexPos> vertices_;
};