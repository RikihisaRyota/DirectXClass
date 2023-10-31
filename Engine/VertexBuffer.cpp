#include "VertexBuffer.h"

#include <cassert>
#include <d3dx12.h>

void VertexBuffer::Create(ID3D12Device* device, std::vector<VertexPos> vertices) {
#pragma region 頂点バッファ
	HRESULT result = S_FALSE;
	vertices_ = vertices;
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPos) * vertices_.size());
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

		// 頂点バッファ生成
		result = device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&vertBuff_));
		assert(SUCCEEDED(result));
	}
	// バッファを転送
	UpLoadData();
#pragma endregion 頂点バッファ
#pragma region 頂点バッファビュー
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);
#pragma endregion 頂点バッファビュー
}

void VertexBuffer::UpLoadData() {
	HRESULT result = S_FALSE;
	// 頂点バッファへのデータ転送
	VertexPos* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}
}