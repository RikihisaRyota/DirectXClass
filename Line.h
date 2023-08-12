#pragma once

#include <d3dx12.h>
#include <vector>

#include "LineGraphicsPipline.h"
#include "cDirectionalLight.h"
#include "cMaterial.h"
#include "ToonGraphicsPipline.h"
#include "WorldTransform.h"
#include "cVertexPos.h"
#include "ViewProjection.h"

class Line {
private:
	struct Vertex {
		Vector4 position_;
		Vector4 color_;
	};
public:
	static Line* GetInstance();
	void Initialize();
	static void SetDevice(ID3D12Device* device);
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	static void PostDraw();
	static void Release();
	void Reset();
	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}
	void Draw(
		const Vector3& v1,
		const Vector3& v2,
		const Vector4& color=Vector4(1.0f,1.0f,1.0f,1.0f));
private:
	void BasicDraw(
		const Vector3& v1,
		const Vector3& v2,
		const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
private:
#pragma region DirectX関連
	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// グラフィックパイプライン
	std::unique_ptr<LineGraphicsPipline> lineGraphicsPipline_ = nullptr;
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<Vertex> vertices_;
#pragma endregion
#pragma region インデックスバッファ
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// 頂点インデックスデータ
	std::vector<uint16_t> indices_;
#pragma endregion
	// 参照するビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
};