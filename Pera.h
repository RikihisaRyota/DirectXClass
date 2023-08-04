#pragma once

#include <d3dx12.h>

class DirectXCommon;
class Pera {
public:
	ID3D12Resource* GetResource() { return peraResource_.Get(); }
	void Initialize(DirectXCommon* dxCommon, ID3D12Resource* buff, ID3D12DescriptorHeap* rtv);
	void PreDraw();
	void PostDraw();
private:
	// 全画面クリア
	void ClearRenderTarget();
private:
	// デバイス
	static DirectXCommon* sDxCommon_;
	Microsoft::WRL::ComPtr<ID3D12Resource> peraResource_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
};