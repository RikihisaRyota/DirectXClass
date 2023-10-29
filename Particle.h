#pragma once

#include <d3dx12.h>
#include <vector>

#include "ParticleGraphicsPipline.h"
#include "cMaterial.h"
#include "WorldTransform.h"
#include "cVertexPos.h"
#include "ViewProjection.h"

class Particle {
public:
	struct TransformationMatrix {
		Matrix4x4 World;
	};
public:
	static void SetDevice(ID3D12Device* device);
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	static void PostDraw();
	static Particle* Create(uint32_t IsLighting = 1, bool IsToon = false);
	void Draw(
		const ViewProjection& viewProjection,
		uint32_t textureHadle = 0u);
	void SetMaterial(const cMaterial& material);
private:
	void Initialize();
	void BasicDraw(
		const ViewProjection& viewProjection,
		uint32_t textureHadle);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
private:
#pragma region DirectX関連
	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// グラフィックパイプライン
	std::unique_ptr<ParticleGraphicsPipline> basicGraphicsPipline_ = nullptr;
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<cVertexPos> vertices_;
#pragma endregion
#pragma region インデックスバッファ
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// 頂点インデックスデータ
	std::vector<uint16_t> indices_;
#pragma endregion
#pragma region マテリアル
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff_;
	// マテリアル
	cMaterial* material_ = nullptr;
#pragma endregion
#pragma region ワールドトランスフォーム
	const uint32_t kNumInstance_ = 10;
	uint32_t descriptorSizeSRV = 0u;
	// ワールドトランスフォームマトリックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingBuff_;
	// ワールドトランスフォーム
	TransformationMatrix* instancingDate_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSRVCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSRVGPUHandle;
#pragma endregion

};

