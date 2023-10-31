#pragma once

#include <d3dx12.h>
#include <vector>

#include "cVertexPos.h"

class Mesh {
public:
	static void SetDevice(ID3D12Device* device) { sDevice = device; }
	void SetVertices(const cVertexPos& vertices) { vertices_.emplace_back(vertices); }
	void Initialize();
	const  D3D12_VERTEX_BUFFER_VIEW* GetVBView() const { return &vbView_; }
	std::vector<cVertexPos> GetVertices() { return vertices_; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
public:
	// デバイス
	static ID3D12Device* sDevice;
private:
#pragma region DirectX関連
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<cVertexPos> vertices_;
#pragma endregion
};