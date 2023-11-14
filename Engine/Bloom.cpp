#include "Bloom.h"

#include <cassert>

#include "DirectXCommon.h"

using namespace Microsoft::WRL;

void Bloom::Initialize(Buffer* original, Buffer* depth, PostEffect* postEffect) {
	// パイプライン生成
	bloomPipeline_ = new BloomPipeline();
	bloomPipeline_->InitializeGraphicsPipeline();
	preBloomPipeline_ = new PreBloomPipeline();
	preBloomPipeline_->InitializeGraphicsPipeline();
	// バッファー
	temporaryBuffer_ = new Buffer();
	originalBuffer_ = original;
	originalDepthBuffer_ = depth;
	CreateResource();
	gaussianBlur_ = new GaussianBlur();
	gaussianBlur_->Initialize(originalBuffer_, originalDepthBuffer_, postEffect);
}

void Bloom::Update() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// リソースバリアの変更
	CD3DX12_RESOURCE_BARRIER barrier[2];
	barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		temporaryBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		originalBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(2, barrier);
	commandList->OMSetRenderTargets(1, &temporaryBuffer_->rtvHandle, false, &originalDepthBuffer_->dpsCPUHandle);

	ClearRenderTarget(temporaryBuffer_->rtvHandle);
	ClearDepthBuffer(originalDepthBuffer_->dpsCPUHandle);

	commandList->SetGraphicsRootSignature(preBloomPipeline_->GetRootSignature());
	commandList->SetPipelineState(preBloomPipeline_->GetPipelineState());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->SetGraphicsRootDescriptorTable(BloomPipeline::ROOT_PARAMETER_TYP::TEXTURE, originalBuffer_->srvGPUHandle);
	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);

	// リソースバリアの変更
	barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		temporaryBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		originalBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(2, barrier);

	for (size_t i = 0; i < 5; i++) {
		gaussianBlur_->Update();
	}
	SetCommandList();
}

void Bloom::Shutdown() {
	gaussianBlur_->Shutdown();
	delete gaussianBlur_;
	indices_.clear();
	idxBuff_.Reset();
	vertices_.clear();
	vertBuff_.Reset();
	temporaryBuffer_->buffer.Reset();
	delete temporaryBuffer_;
	delete bloomPipeline_;
	delete preBloomPipeline_;
}

void Bloom::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
	// 全画面のクリア
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	DirectXCommon::GetInstance()->GetCommandList()->ClearRenderTargetView(handle, clearColor, 0, nullptr);
}

void Bloom::ClearDepthBuffer(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
	// 深度バッファのクリア
	DirectXCommon::GetInstance()->GetCommandList()->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Bloom::CreateResource() {

	auto common = DirectXCommon::GetInstance();
	auto device = common->GetDevice();
	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// シェーダーの計算結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2Dテクスチャとして書き込む
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WinApp::kWindowWidth,
		WinApp::kWindowHeight,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);
	// レンダリング時のクリア値と同じ
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);

	auto result = device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PRESENT, &clearValue,
		IID_PPV_ARGS(&temporaryBuffer_->buffer));
	temporaryBuffer_->buffer->SetName(L"Bloom");
	assert(SUCCEEDED(result));
	common->GetSRVCPUGPUHandle(temporaryBuffer_->srvCPUHandle, temporaryBuffer_->srvGPUHandle);
	temporaryBuffer_->rtvHandle = common->GetRTVCPUDescriptorHandle();
	device->CreateShaderResourceView(temporaryBuffer_->buffer.Get(), &srvDesc, temporaryBuffer_->srvCPUHandle);
	device->CreateRenderTargetView(temporaryBuffer_->buffer.Get(), &rtvDesc, temporaryBuffer_->rtvHandle);

	// VertexBuffer
	vertices_ = {
		{{-1.0f, -1.0f, -1.0f, 1.0f},{0.0f, 1.0f}}, // 左下
		{{-1.0f, +1.0f, -1.0f, 1.0f},{0.0f, 0.0f}}, // 左上
		{{+1.0f, -1.0f, -1.0f, 1.0f},{1.0f, 1.0f}}, // 右下
		{{+1.0f, +1.0f, -1.0f, 1.0f},{1.0f, 0.0f}}, // 右上
	};

	indices_ = {
		0,  1,  3,
		3,  2,  0,
	};
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPos) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		VertexPos* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	// インデックスデータのサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());
	idxBuff_ = CreateBuffer(sizeIB);
	// インデックスバッファへのデータ転送
	uint16_t* indexMap = nullptr;
	result = idxBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		idxBuff_->Unmap(0, nullptr);
	}
	// インデックスバッファビューの作成
	ibView_.BufferLocation = idxBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB; // 修正: インデックスバッファのバイトサイズを代入
}

void Bloom::SetCommandList() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// リソースバリアの変更
	CD3DX12_RESOURCE_BARRIER barrier[2];
	barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		temporaryBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		originalBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(2, barrier);
	commandList->OMSetRenderTargets(1, &temporaryBuffer_->rtvHandle, false, &originalDepthBuffer_->dpsCPUHandle);
	
	commandList->SetGraphicsRootSignature(bloomPipeline_->GetRootSignature());
	commandList->SetPipelineState(bloomPipeline_->GetPipelineState());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->SetGraphicsRootDescriptorTable(BloomPipeline::ROOT_PARAMETER_TYP::TEXTURE, originalBuffer_->srvGPUHandle);
	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);

	// リソースバリアの変更
	barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		temporaryBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		originalBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(2, barrier);

	commandList->OMSetRenderTargets(1, &originalBuffer_->rtvHandle, false, &originalDepthBuffer_->dpsCPUHandle);

	commandList->SetGraphicsRootSignature(bloomPipeline_->GetRootSignature());
	commandList->SetPipelineState(bloomPipeline_->GetPipelineState());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->SetGraphicsRootDescriptorTable(BloomPipeline::ROOT_PARAMETER_TYP::TEXTURE, temporaryBuffer_->srvGPUHandle);
	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);


	CD3DX12_RESOURCE_BARRIER barrier_0=CD3DX12_RESOURCE_BARRIER::Transition(
		temporaryBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier_0);
}
ComPtr<ID3D12Resource> Bloom::CreateBuffer(UINT size) {
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