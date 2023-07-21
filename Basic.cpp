#include "Basic.h"
#include <cassert>
#include <d3dx12.h>
#include <fstream>
#include <sstream>
#include <Windows.h>

#include "ConvertString.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* Basic::sDevice = nullptr;
UINT Basic::sDescriptorHandleIncrementSize;
ComPtr<ID3D12RootSignature> Basic::sRootSignature;
ComPtr<ID3D12PipelineState> Basic::sPipelineState;
ComPtr<ID3D12GraphicsCommandList> Basic::sCommandList;
ComPtr<IDxcUtils> Basic::dxcUtils_;
ComPtr<IDxcCompiler3> Basic::dxcCompiler_;
ComPtr<IDxcIncludeHandler> Basic::includeHandler_;
bool Basic::cullingFlag_;

void Basic::StaticInitialize(ID3D12Device* device, int window_width, int window_height) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

void Basic::InitializeGraphicsPipeline(bool flag) {
	HRESULT hr = S_FALSE;
#pragma region Rootsignature生成
	// Rootsignature設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	descriptorRange[0].BaseShaderRegister = 0;// t0を使用
	descriptorRange[0].NumDescriptors = 1;// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// Offsetを自動計算

	descriptorRange[1].BaseShaderRegister = 1;// t1を使用(二枚目テクスチャ用)
	descriptorRange[1].NumDescriptors = 1;// 数は1つ
	descriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
	descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// Offsetを自動計算

	// rootParameterの生成
	D3D12_ROOT_PARAMETER rootParameters[COUNT] = {};
	rootParameters[WORLDTRANSFORM].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[WORLDTRANSFORM].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[WORLDTRANSFORM].Descriptor.ShaderRegister = WORLDTRANSFORM; //!< レジスタ番号0とバインド

	rootParameters[VIEWPROJECTION].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[VIEWPROJECTION].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[VIEWPROJECTION].Descriptor.ShaderRegister = VIEWPROJECTION; //!< レジスタ番号1とバインド

	rootParameters[TEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameters[TEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootParameters[TEXTURE].DescriptorTable.pDescriptorRanges = &descriptorRange[0]; // 最初のデスクリプタレンジを指定
	rootParameters[TEXTURE].DescriptorTable.NumDescriptorRanges = 1; // 1つのデスクリプタレンジを利用

	rootParameters[TOONTEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameters[TOONTEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootParameters[TOONTEXTURE].DescriptorTable.pDescriptorRanges = &descriptorRange[1]; // 2枚目のデスクリプタレンジを指定
	rootParameters[TOONTEXTURE].DescriptorTable.NumDescriptorRanges = 1; // 1つのデスクリプタレンジを利用

	rootParameters[CULLINGFLUG].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[CULLINGFLUG].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[CULLINGFLUG].Descriptor.ShaderRegister = CULLINGFLUG; //!< レジスタ番号4とバインド

	rootParameters[MATERIAL].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[MATERIAL].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[MATERIAL].Descriptor.ShaderRegister = MATERIAL; //!< レジスタ番号5とバインド

	rootParameters[LIGHTING].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[LIGHTING].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[LIGHTING].Descriptor.ShaderRegister = LIGHTING; //!< レジスタ番号6とバインド

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
	 "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{// 法線
	"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
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
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
#pragma endregion BlendStateの設定
#pragma region RasiterzerStateの設定
	//RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	if (flag) {
		//裏面（時計回り）を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	}
	else {
		//表面（時計回り）を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
	}
	cullingFlag_ = flag;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
#pragma endregion RasiterzerStateの設定
#pragma region ShaderをCompileする
#pragma region dxcCompilerの初期化
	dxcCompilerInitialize();
#pragma endregion dxcCompilerの初期化
	//Shaderをコンパイルする
	ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Basic.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(vertexShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Basic.PS.hlsl",
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
	hr = sDevice->CreateGraphicsPipelineState(&graphicPipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));
#pragma endregion PSOの生成
}

void Basic::dxcCompilerInitialize() {
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

IDxcBlob* Basic::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
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

Basic* Basic::Create(bool flag) {
	// Basicのインスタンスを生成
	Basic* basic = new Basic();
	assert(basic);

	// パイプライン初期化
	basic->InitializeGraphicsPipeline(flag);

	// 初期化
	basic->Initialize();

	return basic;
}

void Basic::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Basic::sCommandList == nullptr);
	// コマンドリストをセット
	sCommandList = cmdList;
	//// パイプラインステートの設定
	//sCommandList->SetPipelineState(sPipelineState.Get());
	//// ルートシグネチャの設定
	//sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	//// プリミティブ形状を設定
	//sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Basic::PostDraw() {
	// コマンドリストの解除
	Basic::sCommandList = nullptr;
}

void Basic::Release() {
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

void Basic::Initialize() {
	// nullptrチェック
	assert(sDevice);

	// 生成
	CreateBasic();
}

void Basic::Draw(
	const WorldTransform& worldTransform, const ViewProjection& viewProjection,
	uint32_t textureHadle) {
	// nullptrチェック
	assert(sDevice);
	assert(sCommandList);
	assert(worldTransform.constBuff_.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());

	sCommandList->SetPipelineState(pipelineState_.Get());
	// プリミティブ形状を設定
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	sCommandList->IASetVertexBuffers(0, 1, &vbView_);

	// インデックスバッファの設定
	sCommandList->IASetIndexBuffer(&ibView_);

	// CBVをセット（ワールド行列）
	sCommandList->SetGraphicsRootConstantBufferView(WORLDTRANSFORM, worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	sCommandList->SetGraphicsRootConstantBufferView(VIEWPROJECTION, viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（カリングフラグ）
	sCommandList->SetGraphicsRootConstantBufferView(CULLINGFLUG, cullingBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	sCommandList->SetGraphicsRootConstantBufferView(MATERIAL, materialBuff_->GetGPUVirtualAddress());

	// DirectionalLight用のCBufferの場所を設定
	sCommandList->SetGraphicsRootConstantBufferView(LIGHTING, directionalLightBuff_->GetGPUVirtualAddress());

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList.Get(), TEXTURE, textureHadle);
	// toonShader用の画像をセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList.Get(), TOONTEXTURE, static_cast<int>(TextureManager::TextureHandle::TOON));

	// 描画コマンド
	sCommandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

void Basic::CreateBasic() {
	HRESULT result = S_FALSE;

	vertices_ = {
		//  x      y      z     w       nx    ny     nz      u     v
		// 前
		{{-1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 左下
		{{-1.0f, +1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 左上
		{{+1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 右下
		{{+1.0f, +1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 右上
		 // 後(前面とZ座標の符号が逆)
		{{+1.0f, -1.0f, +1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}, {0.0f, 1.0f}}, // 左下
		{{+1.0f, +1.0f, +1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}, {0.0f, 0.0f}}, // 左上
		{{-1.0f, -1.0f, +1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}, {1.0f, 1.0f}}, // 右下
		{{-1.0f, +1.0f, +1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}, {1.0f, 0.0f}}, // 右上
		// 左
		{{-1.0f, -1.0f, +1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		{{-1.0f, +1.0f, +1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		{{-1.0f, -1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		{{-1.0f, +1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
		// 右（左面とX座標の符号が逆）
		{{+1.0f, -1.0f, -1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		{{+1.0f, +1.0f, -1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		{{+1.0f, -1.0f, +1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		{{+1.0f, +1.0f, +1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
		// 下
		{{+1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		{{+1.0f, -1.0f, +1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		{{-1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		{{-1.0f, -1.0f, +1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
		// 上（下面とY座標の符号が逆）
		{{-1.0f, +1.0f, -1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		{{-1.0f, +1.0f, +1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		{{+1.0f, +1.0f, -1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		{{+1.0f, +1.0f, +1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
	};

	// 頂点インデックスの設定
	indices_ = { 0,  1,  3,  3,  2,  0,

				4,  5,  7,  7,  6,  4,

				8,  9,  11, 11, 10, 8,

				12, 13, 15, 15, 14, 12,

				16, 17, 19, 19, 18, 16,

				20, 21, 23, 23, 22, 20 };
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
#pragma region マテリアルバッファ
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Material));

		// バッファ生成
		result = sDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&materialBuff_));
		assert(SUCCEEDED(result));
	}

	// マテリアルへのデータ転送
	result = materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingを有効化
	material_->enableLightint = true;
#pragma endregion
#pragma region ライティングバッファ
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(DirectionalLight));

		// バッファ生成
		result = sDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&directionalLightBuff_));
		assert(SUCCEEDED(result));
	}

	// ライティングバッファへのデータ転送
	result = directionalLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	// 初期値代入
	directionalLight_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_->direction = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy = 1.0f;
#pragma endregion
#pragma region カリング
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Culling));

		// バッファ生成
		result = sDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&cullingBuff_));
		assert(SUCCEEDED(result));
	}

	// カリングへのデータ転送
	result = cullingBuff_->Map(0, nullptr, reinterpret_cast<void**>(&culling_));
	culling_->flag = cullingFlag_;
#pragma endregion

}