#pragma once

#include <d3dx12.h>
#include <vector>

#include "BasicGraphicsPipline.h"
#include "Material.h"
#include "Mesh.h"
#include "ToonGraphicsPipline.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class Model {
public:
	static void SetDevice(ID3D12Device* device) { sDevice = device; }
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	static void PostDraw();
	/// <summary>
	/// Create
	/// </summary>
	/// <param name="modelname">model</param>
	/// <returns></returns>
	static Model* Create(const std::string& modelname, uint32_t  IsLighting = 0, uint32_t IsToon = 0);
	void Draw(
		const WorldTransform& worldTransform,
		const ViewProjection& viewProjection,
		uint32_t textureHadle = 0u);
	size_t GetModelSize() const { return meshs_.size(); }
	Material* GetMaterial(size_t i) { return materials_[i].get(); }
	void SetToon(uint32_t IsToon) { IsToon_ = IsToon; }
private:
	void Initialize();
	void BasicDraw(
		const WorldTransform& worldTransform,
		const ViewProjection& viewProjection,
		uint32_t textureHadle,size_t i);
	void ToonDraw(
		const WorldTransform& worldTransform,
		const ViewProjection& viewProjection,
		size_t i);
	// OBJファイルの読み込み
	void LoadObjFile(const std::string& filename);
	// mtlファイルの読み込み
	std::string LoadMaterialTemplateFile(const std::string& filepath, const std::string& filename/*変更検討中*/);
private:
	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// グラフィックパイプライン
	std::unique_ptr<BasicGraphicsPipline> basicGraphicsPipline_ = nullptr;
	std::unique_ptr<ToonGraphicsPipline> toonGraphicsPipline_ = nullptr;
	
	std::vector<std::unique_ptr<Mesh>> meshs_;
	std::vector<std::unique_ptr<Material>> materials_;
	// ライティングするか
	uint32_t IsLighting_;
	// トゥーンシェーディングするか
	uint32_t IsToon_;
};