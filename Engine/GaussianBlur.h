#pragma once
#include <d3d12.h>
#include <d3dx12.h>

#include <memory>

#include "VerticalBlurPipeline.h"

#include "Vector2.h"
#include "Vector4.h"


class GaussianBlur {
private:
	struct Buffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer{};

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};

		D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle{};
	};
	struct VertexPos {
		Vector4 position{};
		Vector2 texcoord{};
	};
public:
	void Initialize();
	void Update();
	void Shutdown();

	ID3D12RootSignature* GetVerticalBlurRootSignature() { return verticalBlurPipelinePipeline_->GetRootSignature(); }
	ID3D12PipelineState* GetVerticalBlurPipelineState() { return verticalBlurPipelinePipeline_->GetPipelineState(); }
	ID3D12Resource* GetBuffer() { return temporaryBuffer_->buffer.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() { return temporaryBuffer_->rtvHandle; }
private:
	void CreateResource();
	void SetCommandList();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
	std::unique_ptr<VerticalBlurPipeline> verticalBlurPipelinePipeline_;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	std::vector<VertexPos> vertices_;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint16_t> indices_;

	// 描画用バッファ
	Buffer* temporaryBuffer_;


}; 