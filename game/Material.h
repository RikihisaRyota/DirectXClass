#pragma once

#include <cstdint>
#include <d3dx12.h>
#include <string>
#include <vector>

#include "cDirectionalLight.h"
#include "cMaterial.h"

class Material {
public:
	static void SetDevice(ID3D12Device* device) { sDevice = device; }

	void SetTextureHandle(const uint32_t textureHandle) { textureHandle_ = textureHandle; }
	uint32_t GetTextureHandle()const { return textureHandle_; }
	ID3D12Resource* GetMaterialBuff() const {return materialBuff_.Get();}
	ID3D12Resource* GeLightingBuff() const {return directionalLightBuff_.Get();}
	void Initialize(uint32_t IsLiting = 0);
	cDirectionalLight* GetDirectionalLight() const { return directionalLight_; }
	void SetDirectionalLight(const cDirectionalLight& DirectionalLight);
	cMaterial* GetMaterial() const { return material_; }
	void SetMaterial(const cMaterial& material);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
private:
	// デバイス
	static ID3D12Device* sDevice;
#pragma region マテリアル
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff_;
	// マテリアル
	cMaterial* material_ = nullptr;
#pragma endregion
#pragma region ライティング
	// ライティングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightBuff_;
	// ライティング
	cDirectionalLight* directionalLight_ = nullptr;
#pragma endregion
	uint32_t textureHandle_;
};

