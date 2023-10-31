#include "PlaneRenderer.h"

#include "TextureManager.h"
#include "MyMath.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* PlaneRenderer::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> PlaneRenderer::cmdList_;

void PlaneRenderer::SetDevice(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

void PlaneRenderer::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
}
void PlaneRenderer::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

PlaneRenderer* PlaneRenderer::Create(uint32_t IsLighting, bool IsToon) {
	// PlaneRendererのインスタンスを生成
	PlaneRenderer* sptite = new PlaneRenderer();
	assert(sptite);
	// パイプライン初期化
	sptite->basicGraphicsPipline_ = std::make_unique<BasicGraphicsPipline>();
	sptite->toonGraphicsPipline_ = std::make_unique<ToonGraphicsPipline>();

	sptite->basicGraphicsPipline_->InitializeGraphicsPipeline();
	sptite->toonGraphicsPipline_->InitializeGraphicsPipeline();
	// ライティングするか
	sptite->IsLighting_ = IsLighting;
	// トゥーンシェーディングするか
	sptite->IsToon_ = IsToon;
	// 初期化
	sptite->Initialize();
	return sptite;
}

void PlaneRenderer::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	if (IsToon_) {
		ToonDraw(worldTransform, viewProjection, textureHadle);
	}
	BasicDraw(worldTransform, viewProjection, textureHadle);
}

void PlaneRenderer::SetToon(uint32_t IsToon) {
	IsToon_ = IsToon;
}

void PlaneRenderer::SetDirectionalLight(const cDirectionalLight& DirectionalLight) {
	directionalLight_->color_ = DirectionalLight.color_;
	directionalLight_->direction_ = Normalize(DirectionalLight.direction_);
	directionalLight_->intensiy_ = DirectionalLight.intensiy_;
	directionalLight_->sharpness_ = DirectionalLight.sharpness_;
}

void PlaneRenderer::SetMaterial(const cMaterial& material) {
	material_->color_ = material.color_;
	material_->enableLightint_ = material.enableLightint_;
	material_->uvTransform_ = material.uvTransform_;
}


void PlaneRenderer::Initialize() {
	HRESULT result = S_FALSE;

	vertices_ = {
		//	x      y     z      w      nx    ny    nz     u     v
		{{-1.0f, -1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{0.0f, 1.0f}}, // 左下 0
		{{-1.0f, +1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{0.0f, 0.0f}}, // 左上 1
		{{+1.0f, +1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{1.0f, 0.0f}}, // 右上 2
		{{+1.0f, -1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{1.0f, 1.0f}}, // 右下 3
	};
	// 頂点インデックスの設定
	indices_ = { 0, 1, 2,
				 0, 2, 3,
	};
#pragma region 頂点バッファ
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(cVertexPos) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		cVertexPos* vertMap = nullptr;
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
#pragma endregion 頂点バッファ
#pragma region インデックスバッファ
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
#pragma endregion インデックスバッファ
#pragma region マテリアルバッファ
	materialBuff_ = CreateBuffer(sizeof(cMaterial));
	// マテリアルへのデータ転送
	result = materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingを有効化
	material_->enableLightint_ = IsLighting_;
	material_->uvTransform_ = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
#pragma endregion
#pragma region ライティングバッファ
	directionalLightBuff_ = CreateBuffer(sizeof(cDirectionalLight));
	// ライティングバッファへのデータ転送
	result = directionalLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	// 初期値代入
	directionalLight_->color_ = { 1.0f,1.0f,1.0f };
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;
#pragma endregion
}

void PlaneRenderer::BasicDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(basicGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(basicGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// インデックスバッファの設定
	cmdList_->IASetIndexBuffer(&ibView_);

	// CBVをセット（ワールド行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::MATERIAL), materialBuff_->GetGPUVirtualAddress());

	// DirectionalLight用のCBufferの場所を設定
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::LIGHTING), directionalLightBuff_->GetGPUVirtualAddress());

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::TEXTURE), textureHadle);

	// 描画コマンド
	cmdList_->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

void PlaneRenderer::ToonDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(toonGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(toonGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// インデックスバッファの設定
	cmdList_->IASetIndexBuffer(&ibView_);

	// CBVをセット（ワールド行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::MATERIAL), materialBuff_->GetGPUVirtualAddress());

	// DirectionalLight用のCBufferの場所を設定
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::LIGHTING), directionalLightBuff_->GetGPUVirtualAddress());

	// SRVをセット
	// トゥーンシェーダー用
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::TOONTEXTURE), static_cast<int>(TextureManager::TextureHandle::TOON));

	// 描画コマンド
	cmdList_->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

ComPtr<ID3D12Resource> PlaneRenderer::CreateBuffer(UINT size) {
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