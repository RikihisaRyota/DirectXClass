#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#include "cVertexPos.h"

#pragma comment(lib,"dxcompiler.lib")
class ToonGraphicsPipline {

public:
	enum class ROOT_PARAMETER_TYP {
		WORLDTRANSFORM,
		VIEWPROJECTION,
		MATERIAL,
		LIGHTING,
		TOONTEXTURE,
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
	ID3D12PipelineState* GetPipelineStatee() { return pipelineState_.Get(); }
private:
	void CreateState();
	void CreateRootsignature();
	void CreateInputLayout();
	void CreateBlendState();
	void CreateRasiterzerState();
	void CreateShaderCompile();
	void CreateDepthStencil();

	void CreatePSO();
	/// <summary>
	/// DXCの初期化
	/// </summary>
	void DXCCompilerInitialize();
	/// <summary>
	/// CompileShader関数
	/// </summary>
	IDxcBlob* CompileShader(
		//ConpilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したのもを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);
private:
	// デバイス
	static ID3D12Device* sDevice;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout_[3];
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
	// dxcCompierを初期化
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
};

