#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#include "cVertexPos.h"

#pragma comment(lib,"dxcompiler.lib")

class VerticalBlurPipeline {
public:
	enum ROOT_PARAMETER_TYP {
		TIME,
		TEXTURE,
		COUNT,
	};
public:
	/// <summary>
	/// �O���t�B�b�N�p�C�v���C���쐬
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
	// ���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	// �C���v�b�g���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout_[2];
	// �u�����h�X�e�[�g
	D3D12_BLEND_DESC blendDesc_{};
	// ���X�^���C�U�[�X�e�[�g
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	// �f�v�X�X�e���V���X�e�[�g
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	// �R���p�C���V�F�[�_�[
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

};

