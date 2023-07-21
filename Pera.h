#pragma once

#include <d3dx12.h>
#include <vector>

#include "PeraGraphicsPipeline.h"
#include "Vector4.h"
#include "Vector2.h"

class Pera {
public:
	struct PeraVertex {
		Vector4 pos_;
		Vector2 texcoord_;
	};
	static void SetDevice(ID3D12Device* device);
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	static void PostDraw();

	void Initialize();

	void Draw();
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
private:
	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// PeraGraphicsPipeline
	std::unique_ptr<PeraGraphicsPipeline> peraGraphicsPipline_ = nullptr;
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<PeraVertex> vertices_;
#pragma endregion
};