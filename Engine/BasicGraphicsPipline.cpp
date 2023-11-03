#include "BasicGraphicsPipline.h"

#include <cassert>

#include "ConvertString.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* BasicGraphicsPipline::sDevice = nullptr;

void BasicGraphicsPipline::SetDevice(ID3D12Device* device) {
	assert(device);
	sDevice = device;
}

void BasicGraphicsPipline::InitializeGraphicsPipeline() {
	CreateState();
	CreatePSO();
}

void BasicGraphicsPipline::CreateState() {
	CreateRootsignature();
	CreateInputLayout();
	CreateBlendState();
	CreateRasiterzerState();
	CreateDepthStencil();
	CreateShaderCompile();
}

void BasicGraphicsPipline::CreateRootsignature() {
	HRESULT hr = S_FALSE;
	// Rootsignature設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;// t0を使用
	descriptorRange[0].NumDescriptors = 1;// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// Offsetを自動計算

	// rootParameterの生成
	D3D12_ROOT_PARAMETER rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::COUNT)] = {};
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::WORLDTRANSFORM)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::WORLDTRANSFORM)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::WORLDTRANSFORM)].Descriptor.ShaderRegister = static_cast<int>(ROOT_PARAMETER_TYP::WORLDTRANSFORM); //!< レジスタ番号0とバインド

	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION)].Descriptor.ShaderRegister = static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION); //!< レジスタ番号1とバインド

	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::MATERIAL)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::MATERIAL)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::MATERIAL)].Descriptor.ShaderRegister = static_cast<int>(ROOT_PARAMETER_TYP::MATERIAL); //!< レジスタ番号2とバインド

	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::LIGHTING)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::LIGHTING)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::LIGHTING)].Descriptor.ShaderRegister = static_cast<int>(ROOT_PARAMETER_TYP::LIGHTING); //!< レジスタ番号3とバインド

	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::TEXTURE)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::TEXTURE)].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::TEXTURE)].DescriptorTable.pDescriptorRanges = &descriptorRange[0]; // 最初のデスクリプタレンジを指定
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::TEXTURE)].DescriptorTable.NumDescriptorRanges = 1; // 1つのデスクリプタレンジを利用

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
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void BasicGraphicsPipline::CreateInputLayout() {
	//InputLayout
	inputLayout_[0] =
	{// xyz座標(1行で書いたほうが見やすい)
	 "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
	inputLayout_[1] =
	{// 法線
	"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
	inputLayout_[2] =
	{// uv座標(1行で書いたほうが見やすい)
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
}

void BasicGraphicsPipline::CreateBlendState() {
	//全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void BasicGraphicsPipline::CreateRasiterzerState() {
	//裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void BasicGraphicsPipline::CreateShaderCompile() {
	DXCCompilerInitialize();
	//Shaderをコンパイルする
	vertexShaderBlob_ = CompileShader(L"Resources/Shaders/Lighting.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(L"Resources/Shaders/Lighting.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(pixelShaderBlob_ != nullptr);
}

void BasicGraphicsPipline::CreateDepthStencil() {
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みをする
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void BasicGraphicsPipline::CreatePSO() {
	HRESULT hr = S_FALSE;
	//PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicPipelineStateDesc{};

	// RootSignature
	graphicPipelineStateDesc.pRootSignature = rootSignature_.Get();

	// InputLayout(Descを書かない方法)
	graphicPipelineStateDesc.InputLayout.pInputElementDescs = inputLayout_;
	graphicPipelineStateDesc.InputLayout.NumElements = _countof(inputLayout_);

	// VertexShader
	graphicPipelineStateDesc.VS = {
		vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize()
	};
	// PixelShader
	graphicPipelineStateDesc.PS = {
		pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize()
	};
	// BlendState
	graphicPipelineStateDesc.BlendState = blendDesc_;

	// Rasterizer
	graphicPipelineStateDesc.RasterizerState = rasterizerDesc_;

	// DepthStencilの設定
	graphicPipelineStateDesc.DepthStencilState = depthStencilDesc_;
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
	hr = sDevice->CreateGraphicsPipelineState(&graphicPipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));
}

void BasicGraphicsPipline::DXCCompilerInitialize() {
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

IDxcBlob* BasicGraphicsPipline::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
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
		//警告、エラーダメ
		assert(false);
	}
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