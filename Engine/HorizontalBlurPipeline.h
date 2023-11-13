#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#pragma comment(lib,"dxcompiler.lib")

class HorizontalBlurPipeline {
public:
	enum ROOT_PARAMETER_TYP {
		PARM,
		TEXTURE,
		COUNT,
	};
public:
	/// <summary>
	/// グラフィックパイプライン作成
	/// </summary>
	void InitializeGraphicsPipeline();
public:
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetPipelineState() { return pipelineState_.Get(); }
private:
	void CreateState();
	void CreateRootSignature();
	void CreateInputLayout();
	void CreateBlendState();
	void CreateRasterizerState();
	void CreateShaderCompile();
	void CreateDepthStencil();

	void CreatePSO();
private:
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout_[2];
	// ブレンドステート
	D3D12_BLEND_DESC blendDesc_{};
	// ラスタライザーステート
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	// デプスステンシルステート
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	// コンパイルシェーダー
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

};

