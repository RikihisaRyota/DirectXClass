#include "Model.h"

#include <cassert>
#include <fstream>
#include <sstream>

#include "TextureManager.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* Model::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Model::cmdList_;

void Model::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
}

void Model::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

Model* Model::Create(const std::string& modelname, uint32_t  IsLighting, uint32_t IsToon) {
	Model* model = new Model();
	// ライティングするか
	model->IsLighting_ = IsLighting;
	// トゥーンシェーディングするか
	model->IsToon_ = IsToon;
	model->LoadObjFile(modelname);
	model->Initialize();
	// パイプライン初期化
	model->basicGraphicsPipline_ = std::make_unique<BasicGraphicsPipline>();
	model->toonGraphicsPipline_ = std::make_unique<ToonGraphicsPipline>();
	model->basicGraphicsPipline_->InitializeGraphicsPipeline();
	model->toonGraphicsPipline_->InitializeGraphicsPipeline();
	return model;
}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	for (size_t i = 0; i < meshs_.size(); i++) {
		if (IsToon_) {
			ToonDraw(worldTransform, viewProjection, i);
		}
		uint32_t tex = textureHadle;
		if (tex == 10) {
			tex = 0;
		}
		else {
			tex = materials_[i]->GetTextureHandle();
		}
		BasicDraw(worldTransform, viewProjection, tex, i);
	}
}

void Model::Initialize() {
	for (auto& mesh : meshs_) {
		mesh->Initialize();
	}
	for (auto& material : materials_) {
		material->Initialize(IsLighting_);
	}
}

void Model::BasicDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle, size_t i) {
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(basicGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(basicGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, meshs_[i]->GetVBView());

	// CBVをセット（ワールド行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::MATERIAL), materials_[i]->GetMaterialBuff()->GetGPUVirtualAddress());

	// DirectionalLight用のCBufferの場所を設定
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::LIGHTING), materials_[i]->GeLightingBuff()->GetGPUVirtualAddress());

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::TEXTURE), textureHadle);

	// 描画コマンド
	cmdList_->DrawInstanced(static_cast<UINT>(meshs_[i]->GetVertices().size()), 1, 0, 0);
}

void Model::ToonDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, size_t i) {
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(toonGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(toonGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, meshs_[i]->GetVBView());

	// CBVをセット（ワールド行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::MATERIAL), materials_[i]->GetMaterialBuff()->GetGPUVirtualAddress());

	// DirectionalLight用のCBufferの場所を設定
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::LIGHTING), materials_[i]->GeLightingBuff()->GetGPUVirtualAddress());

	// SRVをセット
	// トゥーンシェーダー用
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::TOONTEXTURE), static_cast<int>(TextureManager::TextureHandle::TOON));

	// 描画コマンド
	cmdList_->DrawInstanced(static_cast<UINT>(meshs_[i]->GetVertices().size()), 1, 0, 0);
}

void Model::LoadObjFile(const std::string& filename) {
	std::vector<cVertexPos> vertexPos; //!< 構築するModelData
	std::vector<Vector4> positions; //!< 位置
	std::vector<Vector3> normals; //!< 法線
	std::vector<Vector2> texcoords; //!< テクスチャ座標
	std::string line; //!< ファイルから読み込んだ1行を格納するもの

	std::ifstream file("resources/" + filename + "/" + filename + ".obj"); //!< ファイルを開く
	assert(file.is_open()); //!< とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string indentifier;
		std::istringstream s(line);
		s >> indentifier; //!< 先頭の識別子を読む

		// identifierに応じた処理
		if (indentifier == "o") {
			Mesh* mesh = new Mesh();
			meshs_.emplace_back(mesh);
		}
		else if (indentifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f;
			position.w = 1.0f;
			positions.emplace_back(position);
		}
		else if (indentifier == "vt") {
			Vector2 texcood;
			s >> texcood.x >> texcood.y;
			texcood.y = 1.0f - texcood.y;
			texcoords.emplace_back(texcood);
		}
		else if (indentifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.z *= -1.0f;
			normals.emplace_back(normal);
		}
		else if (indentifier == "f") {
			cVertexPos triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinion;
				s >> vertexDefinion;
				// 頂点の要素へのIndexは「位置 / UV / 法線」で格納されているので、
				// 分解してIndexを取得する
				std::istringstream v(vertexDefinion);
				uint32_t elementIdices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); //!< /区切りでインデックスを読んでいく
					if (index.empty()) {
						// テクスチャ座標が省略されている場合、要素に0を代入
						elementIdices[element] = 0;
					}
					else {
						elementIdices[element] = std::stoi(index);
					}
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIdices[0] - 1];
				Vector2 texcoord;
				// UV座標が無ければ{0.0f,0.0f}
				if (texcoords.empty()) {
					texcoord = { 0.0f,0.0f };
				}
				else {
					texcoord = texcoords[elementIdices[1] - 1];
				}
				Vector3 normal = normals[elementIdices[2] - 1];
				triangle[faceVertex] = { position,normal ,texcoord };
			}
			// 頂点を逆順することで、回り順を逆にする
			meshs_.back()->SetVertices(triangle[2]);
			meshs_.back()->SetVertices(triangle[1]);
			meshs_.back()->SetVertices(triangle[0]);
		}
		else if (indentifier == "usemtl") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialName;
			s >> materialName;
			//基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す
			LoadMaterialTemplateFile(filename, materialName);
		}
	}
}

std::string Model::LoadMaterialTemplateFile(const std::string& filepath, const std::string& materialName/*変更検討中*/) {
	// 1. 中で必要となる変数の宣言
	std::string materialData; //!< 構築するMaterialData
	std::string line; //!< ファイルから読み込んだ1行を格納するもの
	// 2. ファイルを開く
	std::ifstream file("resources/" + filepath + "/" + filepath + ".mtl");
	assert(file.is_open());
	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		// identifierに応じた処置
		if (identifier == "newmtl") {
			Material* material = new Material();
			materials_.emplace_back(material);
		}else if(identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData = "resources/" + filepath + "/" + textureFilename;
			materials_.back()->SetTextureHandle(TextureManager::Load(materialData));
		}
	}
	// 4. MateriarDataを返す
	return materialData;
}