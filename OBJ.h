#pragma once

#include <d3dx12.h>
#include <vector>

#include "BasicGraphicsPipline.h"
#include "cDirectionalLight.h"
#include "cMaterial.h"
#include "ToonGraphicsPipline.h"
#include "WorldTransform.h"
#include "cVertexPos.h"
#include "ViewProjection.h"

class OBJ {
public:
	static void SetDevice(ID3D12Device* device);
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	static void PostDraw();
	/// <summary>
	/// 
	/// </summary>
	/// <param name="modelname"></param>
	/// <param name="IsLighting">0:litingなし,1:litingあり,2:Lambert</param>
	/// <param name="IsToon"></param>
	/// <returns></returns>
	static OBJ* Create(const std::string& modelname, uint32_t  IsLighting = 1, bool IsToon = false);
	void Draw(
		const WorldTransform& worldTransform,
		const ViewProjection& viewProjection,
		uint32_t textureHadle = 0u);

	void SetToon(uint32_t IsToon);
	void SetDirectionalLight(const cDirectionalLight& DirectionalLight);
	void SetMaterial(const cMaterial& material);
private:
	void Initialize(const std::string& modelname);
	void BasicDraw(
		const WorldTransform& worldTransform,
		const ViewProjection& viewProjection,
		uint32_t textureHadle);
	void ToonDraw(
		const WorldTransform& worldTransform,
		const ViewProjection& viewProjection,
		uint32_t textureHadle);
	// OBJファイルの読み込み
	std::vector<cVertexPos> LoadObjFile(const std::string& filename);
	// mtlファイルの読み込み
	std::string LoadMaterialTemplateFile(const std::string& filepath, const std::string& filename);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
private:
#pragma region DirectX関連
	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// グラフィックパイプライン
	std::unique_ptr<BasicGraphicsPipline> basicGraphicsPipline_ = nullptr;
	// トゥーンシェーディング用のグラフィックパイプライン
	std::unique_ptr<ToonGraphicsPipline> toonGraphicsPipline_ = nullptr;
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<cVertexPos> vertices_;
#pragma endregion
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
	cDirectionalLight* directionalLight_;
#pragma endregion
#pragma endregion
	// ライティングするか
	uint32_t IsLighting_;
	// トゥーンシェーディングするか
	bool IsToon_;
	// mtlファイルから受け取ったテクスチャハンドル
	uint32_t textureHandle_;
};