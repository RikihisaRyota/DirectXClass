#include "GaussianBlur.h"

#include <cassert>

#include "DirectXCommon.h"

using namespace Microsoft::WRL;

void GaussianBlur::Initialize() {}

void GaussianBlur::Update() {}

void GaussianBlur::Shutdown() {}

void GaussianBlur::CreateResource() {}

void GaussianBlur::SetCommandList() {}

Microsoft::WRL::ComPtr<ID3D12Resource> GaussianBlur::CreateBuffer(UINT size) {
	auto device = DirectXCommon::GetInstance()->GetDevice();
	HRESULT result = S_FALSE;
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	// バッファ生成
	ComPtr<ID3D12Resource> buffer;
	result = device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&buffer));
	assert(SUCCEEDED(result));
	return buffer;
}
