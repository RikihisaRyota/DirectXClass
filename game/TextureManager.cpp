#include "TextureManager.h"

#include <cassert>

#include "ConvertString.h"
#include "DirectXCommon.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
DirectXCommon* TextureManager::device_ = nullptr;
ComPtr<ID3D12DescriptorHeap> TextureManager::srvDescriptorHeap_;

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

uint32_t TextureManager::Load(const std::string& fileName) {
	return TextureManager::GetInstance()->LoadInternal(fileName);
}

void TextureManager::Release() {
	TextureManager::GetInstance()->UnLoadInternal();
}

void TextureManager::PreDraw() {
	// セットするために呼び出すのは効率が悪い
	ID3D12DescriptorHeap* ppHeaps[] = { srvDescriptorHeap_.Get() };

	device_->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void TextureManager::UnLoadInternal() {
	srvDescriptorHeap_.Reset();
	// 全テクスチャを初期化
	for (size_t i = 0; i < kNumDescriptors; i++) {
		textures_[i].resource.Reset();
		textures_[i].cpuDescHandleSRV.ptr = 0;
		textures_[i].gpuDescHandleSRV.ptr = 0;
		textures_[i].name.clear();
		useTable_[i] = false;
	}
}

uint32_t TextureManager::LoadInternal(const std::string& filePath) {
	// どこが空いているか探す
	for (uint32_t i = static_cast<int>(TextureHandle::COUNT); i < kNumDescriptors; i++) {
		if (!useTable_[i]) {
			kNumDescriptorsCount = i;

			useTable_[i] = true;

			// TextureデータをCPUにロード
			DirectX::ScratchImage mipImage = LoadTexture(filePath);

			textures_[kNumDescriptorsCount].name = filePath;

			const DirectX::TexMetadata& metadata = mipImage.GetMetadata();

			// TextureResourceの設定
			textures_[kNumDescriptorsCount].resource = CreateTextureResourec(metadata);

			// TextureResourceをTextureデータに転送
			UploadTextureData(textures_[kNumDescriptorsCount].resource.Get(), mipImage);

			// SRVの作成
			CreateShaderResourceView(metadata, textures_[kNumDescriptorsCount].resource.Get());

			// 解放
			mipImage.Release();

			break;
		}
	}
	return kNumDescriptorsCount;
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
	// テクスチャを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// 元のテクスチャが1x1の場合はミップマップの生成は不要なのでそのまま返す
	if (image.GetMetadata().width == 1 && image.GetMetadata().height == 1) {
		return image;
	}

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	return mipImages;
}


ComPtr<ID3D12Resource> TextureManager::CreateTextureResourec(const DirectX::TexMetadata& metadata) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);// Textureの幅
	resourceDesc.Height = UINT(metadata.height);// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);// 奥行き or 配列textureの配列数
	resourceDesc.Format = metadata.format;// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);// Textureの次元数。普段使っているのは2次元
	// 利用するHeapの設定。非常に特殊な運用02_04exで一般的なケースがある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;// WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;// プロセッサの近くに配置
	// Resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->GetDevice()->CreateCommittedResource(
		&heapProperties,// Heapの設定
		D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定。特になし
		&resourceDesc,// Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,//  
		nullptr,// Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}

void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelsを指定して書くImageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			UINT(img->rowPitch),//1ラインサイズ
			UINT(img->slicePitch)//1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}

void TextureManager::CreateShaderResourceView(const DirectX::TexMetadata& metadata, ID3D12Resource* textureResourec) {
	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	textures_[kNumDescriptorsCount].cpuDescHandleSRV = GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorHandleIncrementSize, kNumDescriptorsCount);
	textures_[kNumDescriptorsCount].gpuDescHandleSRV = GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorHandleIncrementSize, kNumDescriptorsCount);

	// SRVの生成
	device_->GetDevice()->CreateShaderResourceView(textureResourec, &srvDesc, textures_[kNumDescriptorsCount].cpuDescHandleSRV);
}

void TextureManager::CreateShaderResourceView(ID3D12Resource* textureResourec) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Texture1D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// SRVを作成するDescriptorHeapの場所を決める
	textures_[static_cast<int>(TextureHandle::PERA)].cpuDescHandleSRV = GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorHandleIncrementSize, static_cast<int>(TextureHandle::PERA));
	textures_[static_cast<int>(TextureHandle::PERA)].gpuDescHandleSRV = GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorHandleIncrementSize, static_cast<int>(TextureHandle::PERA));

	// SRVの生成
	device_->GetDevice()->CreateShaderResourceView(textureResourec, &srvDesc, textures_[static_cast<int>(TextureHandle::PERA)].cpuDescHandleSRV);
}

void TextureManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle) {
	// シェーダリソースビューをセット
	commandList->SetGraphicsRootDescriptorTable(rootParamIndex, textures_[textureHandle].gpuDescHandleSRV);
}

void TextureManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, ID3D12DescriptorHeap* SRV) {
	// シェーダリソースビューをセット
	commandList->SetGraphicsRootDescriptorTable(rootParamIndex, SRV->GetGPUDescriptorHandleForHeapStart());
}

void TextureManager::Initialize(DirectXCommon* device) {
	HRESULT result = S_FALSE;

	device_ = device;

	// デスクリプタサイズを取得
	descriptorHandleIncrementSize =
		device_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダから見えるように
	descHeapDesc.NumDescriptors = kNumDescriptors; // シェーダーリソースビュー1つ
	result = device_->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap_)); // 生成
	assert(SUCCEEDED(result));

	// 全テクスチャを初期化
	for (size_t i = 0; i < kNumDescriptors; i++) {
		textures_[i].resource.Reset();
		textures_[i].cpuDescHandleSRV.ptr = 0;
		textures_[i].gpuDescHandleSRV.ptr = 0;
		textures_[i].name.clear();
		useTable_[i] = 0;
	}
	// 初期テクスチャ
	const int InitialTexture = 2;
	std::string filePath[InitialTexture]{};
	filePath[0] = ("resources/white1x1.png");
	filePath[1] = ("resources/toon.png");
	for (uint32_t i = 0; i < InitialTexture; i++) {
		kNumDescriptorsCount = i;

		useTable_[i] = true;

		// TextureデータをCPUにロード
		DirectX::ScratchImage mipImage = LoadTexture(filePath[i]);

		textures_[kNumDescriptorsCount].name = filePath[i];

		const DirectX::TexMetadata& metadata = mipImage.GetMetadata();

		// TextureResourceの設定
		textures_[kNumDescriptorsCount].resource = CreateTextureResourec(metadata);

		// TextureResourceをTextureデータに転送
		UploadTextureData(textures_[kNumDescriptorsCount].resource.Get(), mipImage);

		// SRVの作成
		CreateShaderResourceView(metadata, textures_[kNumDescriptorsCount].resource.Get());

		// 解放
		mipImage.Release();
	}

}


D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

const D3D12_RESOURCE_DESC TextureManager::GetResoureDesc(uint32_t textureHandle) {
	assert(textureHandle < textures_.size());
	Texture& texture = textures_.at(textureHandle);
	return texture.resource->GetDesc();
}
