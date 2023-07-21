#include "Sphere.h"

#include "TextureManager.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* Sphere::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Sphere::cmdList_;
bool Sphere::IsLighting_;
bool Sphere::IsToon_;

void Sphere::SetDevice(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

void Sphere::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
}
void Sphere::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

Sphere* Sphere::Create(bool IsLighting, bool IsToon) {
	// Sphereのインスタンスを生成
	Sphere* sphere = new Sphere();
	assert(sphere);
	// パイプライン初期化
	sphere->basicGraphicsPipline_ = std::make_unique<BasicGraphicsPipline>();
	sphere->toonGraphicsPipline_ = std::make_unique<ToonGraphicsPipline>();
	sphere->basicGraphicsPipline_->InitializeGraphicsPipeline();
	sphere->toonGraphicsPipline_->InitializeGraphicsPipeline();
	// ライティングするか
	IsLighting_ = IsLighting;
	// トゥーンシェーディングするか
	IsToon_ = IsToon;
	// 初期化
	sphere->Initialize();
	return sphere;
}

void Sphere::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	if (IsToon_) {
		ToonDraw(worldTransform, viewProjection, textureHadle);
	}
	BasicDraw(worldTransform, viewProjection, textureHadle);
}

void Sphere::SetDirectionalLight(const DirectionalLight& DirectionalLight) {
	directionalLight_->color_ = DirectionalLight.color_;
	//directionalLight_->eye_Position_ = DirectionalLight.eye_Position_;
	directionalLight_->direction_ = Normalize(DirectionalLight.direction_);
	directionalLight_->intensiy_ = DirectionalLight.intensiy_;
	directionalLight_->sharpness_ = DirectionalLight.sharpness_;
}

void Sphere::Initialize() {
	HRESULT result = S_FALSE;

	// 経度分割1つ分の角度
	const float kLonEvery = static_cast<float>(3.14159265358979323846) * 2.0f / kSubdivision;
	// 緯度分割1つ分の角度
	const float kLatEvery = static_cast<float>(3.14159265358979323846) / kSubdivision;
	// 緯度の方向に分割 -M_PI/2 ~ M_PI/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		//現在の緯度
		float lat = static_cast<float>(-3.14159265358979323846) / 2.0f + kLatEvery * latIndex;
		// 経度の方向に分割 0 ~ 2*M_PI
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t strat = (latIndex * kSubdivision + lonIndex) * 6;
			// 現在の経度
			float lon = lonIndex * kLonEvery;

			VertexPos vertex;
			// 頂点データを入力する。基準点a
			// a
			vertex.pos_.x = std::cos(lat) * std::cos(lon);
			vertex.pos_.y = std::sin(lat);
			vertex.pos_.z = std::cos(lat) * std::sin(lon);
			vertex.pos_.w = 1.0f;
			vertex.texcoord_.x = float(lonIndex) / float(kSubdivision);
			vertex.texcoord_.y = 1.0f - float(latIndex) / float(kSubdivision);
			vertex.normal_.x = vertex.pos_.x;
			vertex.normal_.y = vertex.pos_.y;
			vertex.normal_.z = vertex.pos_.z;
			vertices_.push_back(vertex);

			// b
			vertex.pos_.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertex.pos_.y = std::sin(lat + kLatEvery);
			vertex.pos_.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertex.pos_.w = 1.0f;
			vertex.texcoord_.x = float(lonIndex) / float(kSubdivision);
			vertex.texcoord_.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			vertex.normal_.x = vertex.pos_.x;
			vertex.normal_.y = vertex.pos_.y;
			vertex.normal_.z = vertex.pos_.z;
			vertices_.push_back(vertex);

			// c
			vertex.pos_.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertex.pos_.y = std::sin(lat);
			vertex.pos_.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertex.pos_.w = 1.0f;
			vertex.texcoord_.x = float(lonIndex + 1) / float(kSubdivision);
			vertex.texcoord_.y = 1.0f - float(latIndex) / float(kSubdivision);
			vertex.normal_.x = vertex.pos_.x;
			vertex.normal_.y = vertex.pos_.y;
			vertex.normal_.z = vertex.pos_.z;
			vertices_.push_back(vertex);

			// c
			vertex.pos_.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertex.pos_.y = std::sin(lat);
			vertex.pos_.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertex.pos_.w = 1.0f;
			vertex.texcoord_.x = float(lonIndex + 1) / float(kSubdivision);
			vertex.texcoord_.y = 1.0f - float(latIndex) / float(kSubdivision);
			vertex.normal_.x = vertex.pos_.x;
			vertex.normal_.y = vertex.pos_.y;
			vertex.normal_.z = vertex.pos_.z;
			vertices_.push_back(vertex);

			// b
			vertex.pos_.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertex.pos_.y = std::sin(lat + kLatEvery);
			vertex.pos_.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertex.pos_.w = 1.0f;
			vertex.texcoord_.x = float(lonIndex) / float(kSubdivision);
			vertex.texcoord_.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			vertex.normal_.x = vertex.pos_.x;
			vertex.normal_.y = vertex.pos_.y;
			vertex.normal_.z = vertex.pos_.z;
			vertices_.push_back(vertex);

			// d
			vertex.pos_.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			vertex.pos_.y = std::sin(lat + kLatEvery);
			vertex.pos_.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			vertex.pos_.w = 1.0f;
			vertex.texcoord_.x = float(lonIndex + 1) / float(kSubdivision);
			vertex.texcoord_.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			vertex.normal_.x = vertex.pos_.x;
			vertex.normal_.y = vertex.pos_.y;
			vertex.normal_.z = vertex.pos_.z;
			vertices_.push_back(vertex);
			
		}
	}
#pragma region 頂点バッファ
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPos) * kSubdivision * kSubdivision * 6);
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		VertexPos* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);
#pragma endregion 頂点バッファ
#pragma region マテリアルバッファ
	materialBuff_ = CreateBuffer(sizeof(Material));
	// マテリアルへのデータ転送
	result = materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingを有効化
	material_->enableLightint_ = IsLighting_;
#pragma endregion
#pragma region ライティングバッファ
	directionalLightBuff_ = CreateBuffer(sizeof(DirectionalLight));
	// ライティングバッファへのデータ転送
	result = directionalLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	// 初期値代入
	directionalLight_->color_ = { 1.0f,1.0f,1.0f };
	//directionalLight_->eye_Position_ = {0.0f,0.0f,0.0f};
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;
#pragma endregion
}

void Sphere::BasicDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
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
	cmdList_->DrawInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0);
}

void Sphere::ToonDraw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
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
	cmdList_->DrawInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0);
}

ComPtr<ID3D12Resource> Sphere::CreateBuffer(UINT size) {
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