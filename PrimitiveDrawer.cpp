#include "PrimitiveDrawer.h"

#include "TextureManager.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* PrimitiveDrawer::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> PrimitiveDrawer::cmdList_;
UINT PrimitiveDrawer::darwCount = 0;
std::vector<PrimitiveDrawer::Vertex> PrimitiveDrawer::vertices_;
std::vector<uint16_t> PrimitiveDrawer::indices_;

PrimitiveDrawer* PrimitiveDrawer::GetInstance() {
	static PrimitiveDrawer instans;
	return &instans;
}

void PrimitiveDrawer::SetDevice(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

void PrimitiveDrawer::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
	// カウントリセット
	darwCount = 0;
	vertices_.clear();
	indices_.clear();
}
void PrimitiveDrawer::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

void PrimitiveDrawer::Release() {
	GetInstance()->Reset();
}

void PrimitiveDrawer::Reset() {
	cmdList_.Reset();
	lineGraphicsPipline_.reset();
	vertBuff_.Reset();
	idxBuff_.Reset();
}

void PrimitiveDrawer::SetDraw(const Vector3& v1, const Vector3& v2, const Vector4& color) {
	HRESULT result = S_FALSE;
	vertices_.emplace_back(Vertex({ v1.x,v1.y,v1.z,1.0f }, { color }));
	indices_.emplace_back(darwCount);
	darwCount++;
	vertices_.emplace_back(Vertex({ v2.x,v2.y,v2.z,1.0f }, { color }));
	indices_.emplace_back(darwCount);
	SetMappingVertex();
	SetMappingIndex();
}

void PrimitiveDrawer::Draw() {
	GetInstance()->BasicDraw();
}

void PrimitiveDrawer::Initialize() {

	darwCount = 0;

	lineGraphicsPipline_ = std::make_unique<LineGraphicsPipline>();
	lineGraphicsPipline_->InitializeGraphicsPipeline();

	vertices_.resize(kMaxLineCount);
	indices_.resize(kMaxLineCount);
	CreateVertexBuffer();
	CreateIndexBuffer();
}

void PrimitiveDrawer::CreateVertexBuffer() {
#pragma region 頂点バッファ
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	SetMappingVertex();
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);
#pragma endregion 頂点バッファ
}

void PrimitiveDrawer::SetMappingVertex() {
	HRESULT result = S_FALSE;
	// 頂点バッファへのデータ転送
	{
		Vertex* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
}

void PrimitiveDrawer::CreateIndexBuffer() {
#pragma region インデックスバッファ
	// インデックスデータのサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());
	idxBuff_ = CreateBuffer(sizeIB);
	// インデックスバッファへのデータ転送
	SetMappingIndex();
	// インデックスバッファビューの作成
	ibView_.BufferLocation = idxBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB; // 修正: インデックスバッファのバイトサイズを代入
#pragma endregion インデックスバッファ
}

void PrimitiveDrawer::SetMappingIndex() {
	HRESULT result = S_FALSE;
	uint16_t* indexMap = nullptr;
	result = idxBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		idxBuff_->Unmap(0, nullptr);
	}
}

void PrimitiveDrawer::BasicDraw() {
	if (!indices_.empty()) {
		// ルートシグネチャの設定
		cmdList_->SetGraphicsRootSignature(lineGraphicsPipline_->GetRootSignature());

		// パイプラインステートの設定
		cmdList_->SetPipelineState(lineGraphicsPipline_->GetPipelineStatee());

		// プリミティブ形状を設定
		cmdList_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		// 頂点バッファの設定
		cmdList_->IASetVertexBuffers(0, 1, &vbView_);

		// インデックスバッファの設定
		cmdList_->IASetIndexBuffer(&ibView_);

		// CBVをセット（ビュープロジェクション行列）
		cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(LineGraphicsPipline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection_->constBuff_->GetGPUVirtualAddress());

		// 描画コマンド
		cmdList_->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
	}
}

ComPtr<ID3D12Resource> PrimitiveDrawer::CreateBuffer(UINT size) {
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