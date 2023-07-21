#include "Pera.h"

#include <cassert>

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* Pera::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Pera::cmdList_;

void Pera::SetDevice(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

void Pera::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
}

void Pera::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

void Pera::Initialize() {

	peraGraphicsPipline_ = std::make_unique<PeraGraphicsPipeline>();
	peraGraphicsPipline_->InitializeGraphicsPipeline();

	HRESULT result = S_FALSE;

	vertices_ = {
		//   x     y    z    w     u     v
		{{-1.0f,-1.0f,0.1f,1.0f},{0.0f,1.0f}},	// 左下
		{{-1.0f,+1.0f,0.1f,1.0f},{0.0f,0.0f}},	// 左上
		{{+1.0f,-1.0f,0.1f,1.0f},{1.0f,1.0f}},	// 右下
		{{-1.0f,+1.0f,0.1f,1.0f},{0.0f,0.0f}},	// 左上
		{{+1.0f,+1.0f,0.1f,1.0f},{1.0f,0.0f}},	// 右上
		{{+1.0f,-1.0f,0.1f,1.0f},{1.0f,1.0f}},	// 右下
	};
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(PeraVertex) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		PeraVertex* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(PeraVertex);
}

void Pera::Draw() {
	cmdList_->SetGraphicsRootSignature(peraGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(peraGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// 描画コマンド
	cmdList_->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

ComPtr<ID3D12Resource> Pera::CreateBuffer(UINT size) {
	HRESULT result = S_FALSE;
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	// バッファ生成
	ComPtr<ID3D12Resource> buffer;
	result = sDevice->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&buffer));
	assert(SUCCEEDED(result));
	return buffer;
}
