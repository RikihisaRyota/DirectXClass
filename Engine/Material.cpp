#include "Material.h"

#include "MyMath.h"
using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* Material::sDevice = nullptr;

void Material::Initialize(uint32_t IsLiting) {
	HRESULT result = S_FALSE;
#pragma region マテリアルバッファ
	materialBuff_ = CreateBuffer(sizeof(cMaterial));
	// マテリアルへのデータ転送
	result = materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingを有効化
	material_->enableLightint_ = IsLiting;
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

void Material::SetDirectionalLight(const cDirectionalLight& DirectionalLight) {
	directionalLight_->color_ = DirectionalLight.color_;
	directionalLight_->direction_ = Normalize(DirectionalLight.direction_);
	directionalLight_->intensiy_ = DirectionalLight.intensiy_;
}

void Material::SetMaterial(const cMaterial& material) {
	material_->color_ = material.color_;
	material_->enableLightint_ = material.enableLightint_;
	material_->uvTransform_ = material.uvTransform_;
}

ComPtr<ID3D12Resource> Material::CreateBuffer(UINT size) {
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
