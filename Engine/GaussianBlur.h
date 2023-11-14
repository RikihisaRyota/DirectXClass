#pragma once
#include <d3d12.h>
#include <d3dx12.h>

#include <memory>

#include "cBuffer.h"
#include "VerticalBlurPipeline.h"
#include "HorizontalBlurPipeline.h"
#include "PostEffectPipeline.h"
#include "PostEffect.h"
#include "Vector2.h"
#include "Vector4.h"


class GaussianBlur {
	friend class PostEffect;
private:
	enum BufferType {
		kVerticalBlur,
		kHorizontalBlur,

		kCount,
	};
	struct VertexPos {
		Vector4 position{};
		Vector2 texcoord{};
	};
public:
	void Initialize(Buffer* buffer, Buffer* depthBuffer, PostEffect* postEffect);
	void Update();
	void Shutdown();

	ID3D12RootSignature* GetVerticalBlurRootSignature() { return verticalBlurPipelinePipeline_->GetRootSignature(); }
	ID3D12PipelineState* GetVerticalBlurPipelineState() { return verticalBlurPipelinePipeline_->GetPipelineState(); }
	ID3D12RootSignature* GetHorizontalBlurRootSignature() { return horizontalBlurPipeline_->GetRootSignature(); }
	ID3D12PipelineState* GetHorizontalBlurPipelineState() { return horizontalBlurPipeline_->GetPipelineState(); }
	ID3D12Resource* GetBuffer(size_t num) { return buffer_.at(num)->buffer.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(size_t num) { return buffer_.at(num)->rtvHandle; }
private:
	static const uint32_t kNumWeights = 8;

	void UpdateWeightTable(float blurPower);
	void CreateResource();
	void SetCommandList();
	void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void ClearDepthBuffer(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
	VerticalBlurPipeline* verticalBlurPipelinePipeline_;
	HorizontalBlurPipeline* horizontalBlurPipeline_;
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

	std::vector<Buffer*> buffer_;
	
	PostEffectGraphicsPipeline* originalPipeline_;
	Buffer* originalBuffer_;
	Buffer* depthBuffer_;

	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
	float weights_[kNumWeights]{};

	PostEffect* postEffect_;
}; 