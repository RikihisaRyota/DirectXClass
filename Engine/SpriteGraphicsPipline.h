#pragma once

#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#include "cVertexPos.h"

#pragma comment(lib,"dxcompiler.lib")

class SpriteGraphicsPipline {
public:
	enum class ROOT_PARAMETER_TYP {
		MATERIAL,
		TEXTURE,
		COUNT,
	};
public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void SetDevice(ID3D12Device* device);
	/// <summary>
	/// グラフィックパイプライン作成
	/// </summary>
	void InitializeGraphicsPipeline();
public:
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetPipelineState() { return pipelineState_.Get(); }
private:
	void CreateState();
	void CreateRootsignature();
	void CreateInputLayout();
	void CreateBlendState();
	void CreateRasiterzerState();
	void CreateShaderCompile();
	void CreateDepthStencil();

	void CreatePSO();
private:
	// デバイス
	static ID3D12Device* sDevice;
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