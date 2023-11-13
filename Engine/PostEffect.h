#pragma once
#include <d3d12.h>
#include <d3dx12.h>

#include <memory>

#include "cBuffer.h"
#include "PostEffectPipeline.h"

class PostEffect {
	struct VertexPos {
		Vector4 position{};
		Vector2 texcoord{};
	};

	struct Time {
		float time;
	};

public:
	void Initialize();
	void Update();
	void Shutdown();

	ID3D12RootSignature* GetRootSignature() { return postEffectPipeline_->GetRootSignature(); }
	ID3D12PipelineState* GetPipelineState() { return postEffectPipeline_->GetPipelineState(); }
	PostEffectGraphicsPipeline* GetPostEffectPipeline() { return postEffectPipeline_.get(); }
	ID3D12Resource* GetBufferResource(){return temporaryBuffer_->buffer.Get();}
	Buffer* GetBuffer() { return temporaryBuffer_; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() { return temporaryBuffer_->rtvHandle; }
private:
	void CreateResource();
	void SetCommandList();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
	std::unique_ptr<PostEffectGraphicsPipeline> postEffectPipeline_;
	// 描画用バッファ
	Buffer* temporaryBuffer_;

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
#pragma region ConstantBuffer
	// 時間
	Microsoft::WRL::ComPtr<ID3D12Resource> timeBuff_;
	Time* time_;
	float count_;
	float countMax_;
#pragma endregion
};

