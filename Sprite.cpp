#include "Sprite.h"
#include <cassert>
#include <d3dx12.h>
#include "ConvertString.h"
#include "TextureManager.h"
#include <Windows.h>

using namespace DirectX;
using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* Sprite::sDevice = nullptr;
UINT Sprite::sDescriptorHandleIncrementSize;
ComPtr<ID3D12RootSignature> Sprite::sRootSignature;
ComPtr<ID3D12PipelineState> Sprite::sPipelineState;
ComPtr<ID3D12GraphicsCommandList> Sprite::sCommandList;
ComPtr<IDxcUtils> Sprite::dxcUtils_;
ComPtr<IDxcCompiler3> Sprite::dxcCompiler_;
ComPtr<IDxcIncludeHandler> Sprite::includeHandler_;

void Sprite::StaticInitialize(ID3D12Device* device, int window_width, int window_height, const std::wstring& directoryPath) {
	// nullptrチェック
	assert(device);

	sDevice = device;

	// パイプライン初期化
	InitializeGraphicsPipeline();
}

void Sprite::InitializeGraphicsPipeline() {
	HRESULT hr = S_FALSE;
#pragma region Rootsignature生成
	// Rootsignature設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;// 0から始める
	descriptorRange[0].NumDescriptors = 1;// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// Offsetを自動計算

	// rootParameterの生成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; //!< レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 1; //!< レジスタ番号1とバインド

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);// tableで利用する数

	descriptionRootSignature.pParameters = rootParameters; //!< ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //!< 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //!< バイリニアフィルタ
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //!< 比較しない
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSampler[0].ShaderRegister = 0; //!< レジスタ番号0を使う
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //!< PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSampler;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);

	// シリアライズしてバイナリする
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元にRootSignatureを生成
	hr = sDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature));
	assert(SUCCEEDED(hr));
#pragma endregion Rootsignature生成	
#pragma region InputLayoutの設定
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{// xyz座標(1行で書いたほうが見やすい)
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{// uv座標(1行で書いたほうが見やすい)
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	};
#pragma endregion InputLayoutの設定
#pragma region BlendStateの設定
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
#pragma endregion BlendStateの設定
#pragma region RasiterzerStateの設定
	//RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
#pragma endregion RasiterzerStateの設定
#pragma region ShaderをCompileする
#pragma region dxcCompilerの初期化
	dxcCompilerInitialize();
#pragma endregion dxcCompilerの初期化
	//Shaderをコンパイルする
	ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Sprite.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(vertexShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Sprite.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(pixelShaderBlob != nullptr);
#pragma endregion ShaderをCompileする
#pragma region DepthStencilの設定
	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをする
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
#pragma endregion DepthStencilの設定
#pragma region PSOの生成
	//PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicPipelineStateDesc{};

	// RootSignature
	graphicPipelineStateDesc.pRootSignature = sRootSignature.Get();

	// InputLayout(Descを書かない方法)
	graphicPipelineStateDesc.InputLayout.pInputElementDescs = inputLayout;
	graphicPipelineStateDesc.InputLayout.NumElements = _countof(inputLayout);

	// VertexShader
	graphicPipelineStateDesc.VS = {
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize()
	};

	// PixelShader
	graphicPipelineStateDesc.PS = {
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize()
	};

	// BlendState
	graphicPipelineStateDesc.BlendState = blendDesc;

	// Rasterizer
	graphicPipelineStateDesc.RasterizerState = rasterizerDesc;

	// DepthStencilの設定
	graphicPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicPipelineStateDesc.NumRenderTargets = 1;
	graphicPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するとポロジ（形状）のタイプ、三角形
	graphicPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// どのように画面に色を打ち込むのか設定（気にしなくてもよい）
	graphicPipelineStateDesc.SampleDesc.Count = 1;
	graphicPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// グラフィックスパイプラインの生成
	hr = sDevice->CreateGraphicsPipelineState(&graphicPipelineStateDesc, IID_PPV_ARGS(&sPipelineState));
	assert(SUCCEEDED(hr));
#pragma endregion PSOの生成
}

void Sprite::dxcCompilerInitialize() {
	HRESULT hr = S_FALSE;
	// dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を行っている
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

IDxcBlob* Sprite::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompilerShader, Path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読み込む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コード

	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外はしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバック用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイラオプション
		_countof(arguments),//コンパイラオプションの数
		includeHandler,//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイラ結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//警告、エラーが出たらログを出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		shaderError->Release();
		//警告、エラーダメ
		assert(false);
	}
	shaderError->Release();
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

Sprite* Sprite::Create() {
	// Basicのインスタンスを生成
	Sprite* sprite = new Sprite();
	assert(sprite);

	// 初期化
	sprite->Initialize();

	return sprite;
}

void Sprite::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Sprite::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = cmdList;

	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	// プリミティブ形状を設定
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Sprite::PostDraw() {
	// コマンドリストの解除
	Sprite::sCommandList = nullptr;
}

void Sprite::Release() {
	// dxcCompierを初期化
	dxcUtils_.Reset();
	dxcCompiler_.Reset();
	includeHandler_.Reset();
	// コマンドリスト
	sCommandList.Reset();
	// パイプラインステートオブジェクト
	sPipelineState.Reset();
	// ルートシグネチャ
	sRootSignature.Reset();
}

void Sprite::Initialize() {
	// nullptrチェック
	assert(sDevice);

	// 生成
	CreateSprite();
}

void Sprite::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {
	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);
	assert(worldTransform.constBuff_.Get());

	// 頂点バッファの設定
	sCommandList->IASetVertexBuffers(0, 1, &vbView_);

	// インデックスバッファの設定
	sCommandList->IASetIndexBuffer(&ibView_);

	// CBVをセット（ワールド行列）
 	sCommandList->SetGraphicsRootConstantBufferView(0, worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	sCommandList->SetGraphicsRootConstantBufferView(1, viewProjection.constBuff_->GetGPUVirtualAddress());

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList.Get(), 2, textureHadle);

	// 描画コマンド
	sCommandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

void Sprite::Delete() {
	// 頂点バッファ
	vertBuff_->Release();
	// インデックスバッファ
	idxBuff_->Release();
	// 頂点データ配列
	vertices_.clear();
	// 頂点インデックスデータ
	indices_.clear();
}

void Sprite::CreateSprite() {
	HRESULT result = S_FALSE;

	vertices_ = {
		// 前
			{{-1.0f, -1.0f, 0.0f},{0.0f, 1.0f}}, // 左下 0
			{{-1.0f, +1.0f, 0.0f},{0.0f, 0.0f}}, // 左上 1
			{{+1.0f, +1.0f, 0.0f},{1.0f, 0.0f}}, // 右上 2
			{{+1.0f, -1.0f, 0.0f},{1.0f, 1.0f}}, // 右下 3
	};
	// 頂点インデックスの設定
	indices_ = { 0, 1, 2,
				 0, 2, 3,
				};



#pragma region 頂点バッファ
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPos) * vertices_.size());
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

		// 頂点バッファ生成
		result = sDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&vertBuff_));
		assert(SUCCEEDED(result));
	}

	// 頂点バッファへのデータ転送
	{
		VertexPos* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
#pragma endregion 頂点バッファ
#pragma region 頂点バッファビュー
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);
#pragma endregion 頂点バッファビュー
#pragma region インデックスバッファ
	// インデックスデータのサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());

	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);

		// インデックスバッファ生成
		result = sDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&idxBuff_));
		assert(SUCCEEDED(result));
	}

	// インデックスバッファへのデータ転送
	uint16_t* indexMap = nullptr;
	result = idxBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);

		idxBuff_->Unmap(0, nullptr);
	}
#pragma endregion インデックスバッファ
#pragma region インデックスバッファビュー
	// インデックスバッファビューの作成
	ibView_.BufferLocation = idxBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;
#pragma endregion インデックスバッファビュー
}
