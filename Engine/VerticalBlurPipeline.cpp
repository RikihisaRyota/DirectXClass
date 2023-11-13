#include "VerticalBlurPipeline.h"

#include <d3dx12.h>
#include <cassert>

#include "ConvertString.h"
#include "DirectXCommon.h"
#include "ShaderCompiler.h"
using namespace Microsoft::WRL;

void VerticalBlurPipeline::InitializeGraphicsPipeline() {
	CreateState();
	CreatePSO();
}

void VerticalBlurPipeline::CreateState() {
	CreateRootSignature();
	CreateInputLayout();
	CreateBlendState();
	CreateRasterizerState();
	CreateDepthStencil();
	CreateShaderCompile();
}

void VerticalBlurPipeline::CreateRootSignature() {
	HRESULT hr = S_FALSE;
	CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[ROOT_PARAMETER_TYP::COUNT]{};
	rootParameters[ROOT_PARAMETER_TYP::TIME].InitAsConstantBufferView(ROOT_PARAMETER_TYP::TIME, 0);
	rootParameters[ROOT_PARAMETER_TYP::TEXTURE].InitAsDescriptorTable(_countof(ranges), ranges);

	CD3DX12_STATIC_SAMPLER_DESC staticSampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_DEFAULT_MIP_LOD_BIAS, D3D12_DEFAULT_MAX_ANISOTROPY,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.pParameters = rootParameters;
	desc.NumParameters = _countof(rootParameters);
	desc.pStaticSamplers = &staticSampler;
	desc.NumStaticSamplers = 1;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// �V���A���C�Y���ăo�C�i������
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&desc,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// �o�C�i��������RootSignature�𐶐�
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void VerticalBlurPipeline::CreateInputLayout() {
	//InputLayout
	inputLayout_[0] =
	{// xyz���W(1�s�ŏ������ق������₷��)
	 "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};

	inputLayout_[1] =
	{// uv���W(1�s�ŏ������ق������₷��)
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
}

void VerticalBlurPipeline::CreateBlendState() {
	//�S�Ă̐F�v�f����������
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void VerticalBlurPipeline::CreateRasterizerState() {
	//���ʁi���v���j��\�����Ȃ�
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//�O�p�`�̒���h��Ԃ�
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void VerticalBlurPipeline::CreateShaderCompile() {
	//Shader���R���p�C������
	vertexShaderBlob_ = ShaderCompiler::Compile(
		L"Resources/Shaders/PostEffect.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = ShaderCompiler::Compile(
		L"Resources/Shaders/PostEffect.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void VerticalBlurPipeline::CreateDepthStencil() {
	// Depth�̋@�\��L��������
	depthStencilDesc_.DepthEnable = true;
	// �������݂�����
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// ��r�֐���LessEqual�B�܂�A�߂���Ε`�悳���
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void VerticalBlurPipeline::CreatePSO() {
	HRESULT hr = S_FALSE;
	//PSO����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicPipelineStateDesc{};

	// RootSignature
	graphicPipelineStateDesc.pRootSignature = rootSignature_.Get();

	// InputLayout
	graphicPipelineStateDesc.InputLayout.pInputElementDescs = inputLayout_;
	graphicPipelineStateDesc.InputLayout.NumElements = _countof(inputLayout_);

	// VertexShader
	graphicPipelineStateDesc.VS = {
		vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize()
	};
	// PixelShader
	graphicPipelineStateDesc.PS = {
		pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize()
	};
	// BlendState
	graphicPipelineStateDesc.BlendState = blendDesc_;

	// Rasterizer
	graphicPipelineStateDesc.RasterizerState = rasterizerDesc_;

	// DepthStencil�̐ݒ�
	graphicPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// ��������RTV�̏��
	graphicPipelineStateDesc.NumRenderTargets = 1;
	graphicPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// ���p����ƃ|���W�i�`��j�̃^�C�v�A�O�p�`
	graphicPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// �ǂ̂悤�ɉ�ʂɐF��ł����ނ̂��ݒ�i�C�ɂ��Ȃ��Ă��悢�j
	graphicPipelineStateDesc.SampleDesc.Count = 1;
	graphicPipelineStateDesc.SampleDesc.Quality = 0;
	graphicPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �O���t�B�b�N�X�p�C�v���C���̐���
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicPipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));
}