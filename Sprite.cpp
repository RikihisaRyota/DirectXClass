#include "Sprite.h"

#include "TextureManager.h"
#include "WinApp.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* Sprite::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Sprite::cmdList_;
Matrix4x4 Sprite::sMatProjection;

void Sprite::SetDevice(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

void Sprite::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
}
void Sprite::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

Sprite* Sprite::Create(
	uint32_t textureHandle, const Vector2& position, const Vector4& color,const Vector2& anchorpoint, bool isFlipX , bool isFlipY) {
	// Spriteのインスタンスを生成
	Sprite* sptite = new Sprite();
	assert(sptite);
	// 仮サイズ
	Vector2 size = { 100.0f, 100.0f };
	{
		// テクスチャ情報取得
		const D3D12_RESOURCE_DESC& resDesc =
			TextureManager::GetInstance()->GetResoureDesc(textureHandle);
		// スプライトのサイズをテクスチャのサイズに設定
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	// パイプライン初期化
	sptite->spriteGraphicsPipline_ = std::make_unique<SpriteGraphicsPipline>();
	sptite->spriteGraphicsPipline_->InitializeGraphicsPipeline();
	
	// 初期化
	sptite->Initialize(textureHandle,position, size, color , anchorpoint , isFlipX , isFlipY );
	return sptite;
}

void Sprite::Draw() {
	BasicDraw();
}

void Sprite::Initialize(uint32_t textureHandle, const Vector2& position, const Vector2& size, const Vector4& color,
	const Vector2& anchorpoint, bool isFlipX, bool isFlipY) {
	HRESULT result = S_FALSE;


	position_ = position;
	size_ = size;
	anchorPoint_ = anchorpoint;
	matWorld_ = MakeIdentity4x4();
	color_ = color;
	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
	texSize_ = size;

	//vertices_ = {
	//	//	x      y     z      w      u     v
	//	{{-0.5f, -0.5f, 0.0f, +1.0f},{0.0f, 1.0f}}, // 左下 0
	//	{{-0.5f, +0.5f, 0.0f, +1.0f},{0.0f, 0.0f}}, // 左上 1
	//	{{+0.5f, +0.5f, 0.0f, +1.0f},{1.0f, 0.0f}}, // 右上 2
	//	{{+0.5f, -0.5f, 0.0f, +1.0f},{1.0f, 1.0f}}, // 右下 3
	//};

	vertices_.resize(4);

	// 頂点インデックスの設定
	indices_ = { 0, 1, 2,
				 0, 2, 3,
	};
#pragma region 頂点バッファ
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosUv) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		VertexPosUv* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
	TransferVertices();
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
	materialBuff_ = CreateBuffer(sizeof(ConstBufferData));
	// マテリアルへのデータ転送
	result = materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
	constMap->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	constMap->mat_ = MakeIdentity4x4();
#pragma endregion
	sMatProjection = MakeOrthographicMatrix(0.0f,0.0f, (float)WinApp::kWindowWidth, (float)WinApp::kWindowHeight, 0.0f, 1.0f);
}

void Sprite::BasicDraw() {
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(spriteGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(spriteGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// ワールド行列の更新
	matWorld_ = MakeIdentity4x4();
	matWorld_ *= MakeRotateZMatrix(rotation_);
	matWorld_ *= MakeTranslateMatrix(Vector3(position_.x, position_.y, 0.0f));

	// 定数バッファにデータ転送
	constMap->color_ = color_;
	constMap->mat_ = matWorld_ * sMatProjection; // 行列の合成

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// インデックスバッファの設定
	cmdList_->IASetIndexBuffer(&ibView_);

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(SpriteGraphicsPipline::ROOT_PARAMETER_TYP::MATERIAL), materialBuff_->GetGPUVirtualAddress());

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(SpriteGraphicsPipline::ROOT_PARAMETER_TYP::TEXTURE), textureHandle_);

	// 描画コマンド
	cmdList_->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

void Sprite::SetTextureHandle(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);
}

void Sprite::SetRotation(float rotation) {
	rotation_ = rotation;

	// 頂点バッファへのデータ転送
	TransferVertices();
}

void Sprite::SetPosition(const Vector2& position) {
	position_ = position;

	// 頂点バッファへのデータ転送
	TransferVertices();
}

void Sprite::SetSize(const Vector2& size) {
	size_ = size;

	// 頂点バッファへのデータ転送
	TransferVertices();
}

void Sprite::SetAnchorPoint(const Vector2& anchorpoint) {
	anchorPoint_ = anchorpoint;

	// 頂点バッファへのデータ転送
	TransferVertices();
}

void Sprite::SetIsFlipX(bool isFlipX) {
	isFlipX_ = isFlipX;

	// 頂点バッファへのデータ転送
	TransferVertices();
}

void Sprite::SetIsFlipY(bool isFlipY) {
	isFlipY_ = isFlipY;

	// 頂点バッファへのデータ転送
	TransferVertices();
}

void Sprite::SetTextureRect(const Vector2& texBase, const Vector2& texSize) {
	texBase_ = texBase;
	texSize_ = texSize;

	// 頂点バッファへのデータ転送
	TransferVertices();
}

ComPtr<ID3D12Resource> Sprite::CreateBuffer(UINT size) {
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

void Sprite::TransferVertices() {
	HRESULT result = S_FALSE;

	// 左下、左上、右下、右上
	enum { LB, LT, RT, RB  };

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	if (isFlipX_) { // 左右入れ替え
		left = -left;
		right = -right;
	}

	if (isFlipY_) { // 上下入れ替え
		top = -top;
		bottom = -bottom;
	}

	vertices_[LB].pos_ = { left, bottom, 0.0f , 1.0f };  // 左下
	vertices_[LT].pos_ = { left, top, 0.0f , 1.0f };     // 左上
	vertices_[RT].pos_ = { right, top, 0.0f , 1.0f }; // 右上
	vertices_[RB].pos_ = { right, bottom, 0.0f , 1.0f };    // 右下

	// テクスチャ情報取得
	{
		float tex_left = texBase_.x / resourceDesc_.Width;
		float tex_right = (texBase_.x + texSize_.x) / resourceDesc_.Width;
		float tex_top = texBase_.y / resourceDesc_.Height;
		float tex_bottom = (texBase_.y + texSize_.y) / resourceDesc_.Height;

		vertices_[LB].uv_ = { tex_left, tex_bottom };  // 左下
		vertices_[LT].uv_ = { tex_left, tex_top };     // 左上
		vertices_[RT].uv_ = { tex_right, tex_top }; // 右上
		vertices_[RB].uv_ = { tex_right, tex_bottom };    // 右下
	}

	// 頂点バッファへのデータ転送
	{
		VertexPosUv* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
}

void Sprite::testUpdate() {
	HRESULT result = S_FALSE;

	// 頂点バッファへのデータ転送
	{
		VertexPosUv* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
}