#include "OBJ.h"

#include <cassert>
#include <d3dx12.h>
#include <fstream>
#include <sstream>
#include <Windows.h>

#include "ConvertString.h"
#include "TextureManager.h"
#include "MyMath.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* OBJ::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> OBJ::cmdList_;

void OBJ::SetDevice(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

OBJ* OBJ::Create(const std::string& modelname, uint32_t IsLighting, bool IsToon) {
	// OBJのインスタンスを生成
	OBJ* obj = new OBJ();
	assert(obj);

	// パイプライン初期化
	obj->basicGraphicsPipline_ = std::make_unique<BasicGraphicsPipline>();
	obj->toonGraphicsPipline_ = std::make_unique<ToonGraphicsPipline>();
	obj->basicGraphicsPipline_->InitializeGraphicsPipeline();
	obj->toonGraphicsPipline_->InitializeGraphicsPipeline();
	// ライティングするか
	obj->IsLighting_ = IsLighting;
	// トゥーンシェーディングするか
	obj->IsToon_ = IsToon;
	// 初期化
	obj->Initialize(modelname);
	return obj;
}


void OBJ::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
}

void OBJ::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

void OBJ::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	if (IsToon_) {
		ToonDraw(worldTransform, viewProjection, textureHandle_);
	}
	uint32_t tex = textureHadle;
	if (tex == 10) {
		tex = 0;
	}
	else {
		tex = textureHandle_;
	}
	BasicDraw(worldTransform, viewProjection, tex);
}

void OBJ::SetDirectionalLight(const cDirectionalLight& DirectionalLight) {
	directionalLight_->color_ = DirectionalLight.color_;
	directionalLight_->direction_ = Normalize(DirectionalLight.direction_);
	directionalLight_->intensiy_ = DirectionalLight.intensiy_;
	directionalLight_->sharpness_ = DirectionalLight.sharpness_;
}

void OBJ::SetMaterial(const cMaterial& material) {
	material_->color_ = material.color_;
	material_->enableLightint_ = material.enableLightint_;
	material_->uvTransform_ = material.uvTransform_;
}

void OBJ::Initialize(const std::string& modelname) {
	HRESULT result = S_FALSE;

	vertices_ = LoadObjFile(modelname);

#pragma region 頂点バッファ
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(cVertexPos) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		cVertexPos* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(cVertexPos);
#pragma endregion 頂点バッファ
#pragma region マテリアルバッファ
	materialBuff_ = CreateBuffer(sizeof(cMaterial));
	// マテリアルへのデータ転送
	result = materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingを有効化
	material_->enableLightint_ = IsLighting_;
	material_->uvTransform_ = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
#pragma endregion
#pragma region ライティングバッファ
	directionalLightBuff_ = CreateBuffer(sizeof(cDirectionalLight));
	// ライティングバッファへのデータ転送
	result = directionalLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	// 初期値代入
	directionalLight_->color_ = { 1.0f,1.0f,1.0f };
	//directionalLight_->eye_Position_ = { 0.0f,0.0f,0.0f };
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;
#pragma endregion
}

void OBJ::BasicDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {

	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(basicGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(basicGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// CBVをセット（ワールド行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::MATERIAL), materialBuff_->GetGPUVirtualAddress());

	// DirectionalLight用のCBufferの場所を設定
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::LIGHTING), directionalLightBuff_->GetGPUVirtualAddress());

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(BasicGraphicsPipline::ROOT_PARAMETER_TYP::TEXTURE), textureHadle);

	// 描画コマンド
	cmdList_->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

void OBJ::ToonDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {

	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(toonGraphicsPipline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(toonGraphicsPipline_->GetPipelineStatee());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// CBVをセット（ワールド行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::MATERIAL), materialBuff_->GetGPUVirtualAddress());

	// DirectionalLight用のCBufferの場所を設定
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::LIGHTING), directionalLightBuff_->GetGPUVirtualAddress());

	// SRVをセット
	// トゥーンシェーダー用
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(ToonGraphicsPipline::ROOT_PARAMETER_TYP::TOONTEXTURE), static_cast<int>(TextureManager::TextureHandle::TOON));

	// 描画コマンド
	cmdList_->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

void OBJ::SetToon(uint32_t IsToon) {
	IsToon_ = IsToon;
}


std::vector<cVertexPos> OBJ::LoadObjFile(const std::string& filename) {
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
		if (indentifier == "v") {
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
					elementIdices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIdices[0] - 1];
				Vector2 texcoord = texcoords[elementIdices[1] - 1];
				Vector3 normal = normals[elementIdices[2] - 1];
				triangle[faceVertex] = { position,normal ,texcoord };
			}
			// 頂点を逆順することで、回り順を逆にする
			vertexPos.push_back(triangle[2]);
			vertexPos.push_back(triangle[1]);
			vertexPos.push_back(triangle[0]);
		}
		else if (indentifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す
			textureHandle_ = TextureManager::Load(LoadMaterialTemplateFile(filename, materialFilename));
		}
	}
	return vertexPos;
}

std::string OBJ::LoadMaterialTemplateFile(const std::string& filepath, const std::string& filename) {
	// 1. 中で必要となる変数の宣言
	std::string materialData; //!< 構築するMaterialData
	std::string line; //!< ファイルから読み込んだ1行を格納するもの
	// 2. ファイルを開く
	std::ifstream file("resources/" + filepath + "/" + filename);
	assert(file.is_open());
	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		// identifierに応じた処置
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData = "resources/" + filepath + "/" + textureFilename;
		}
	}
	// 4. MateriarDataを返す
	return materialData;
}

ComPtr<ID3D12Resource> OBJ::CreateBuffer(UINT size) {
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