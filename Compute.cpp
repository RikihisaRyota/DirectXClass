#include "Compute.h"

#include <assert.h>
#include <d3dx12.h>

#include "ConvertString.h"
#include "DXCCompiler.h"

using namespace Microsoft::WRL;

ID3D12Device* Compute::sDevice_ = nullptr;

Compute::Compute() {}

Compute::~Compute() {
	if (particleBuffer_) {
		particleBuffer_->Release();
	}
    sDevice_->Release();
}

void Compute::SetDevice(ID3D12Device* device) {
	assert(device);
	sDevice_ = device;
}

void Compute::Initialize() {
    InitializeParticle();
    CreateParticleRootSignature();
    ParticleShaderCompile();
    CreateParticleBuffer();
    CreateParticlePipelineState();
    render_ = std::make_unique<BasicGraphicsPipline>();
    render_->InitializeGraphicsPipeline();
}

void Compute::Update(ID3D12GraphicsCommandList* commandList) {
    // Compute Shaderをディスパッチするコードをここに追加
    commandList->SetComputeRootShaderResourceView(0, particleBufferView_.Buffer.FirstElement);
    // スレッドグループの数を計算
    const int groupCountX = (kParticleMax + kThreadsPerGroup - 1) / kThreadsPerGroup;
    const int groupCountY = 1; // 1Dのスレッドグループを使用
    const int groupCountZ = 1;
    commandList->Dispatch(groupCountX, groupCountY, groupCountZ);
    // 結果をCPUにダウンロードするコードをここに追加
    Particle* pParticleData;
    particleBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&pParticleData));
    memcpy(pParticleData, particles_.data(), sizeof(Particle) * kParticleMax);
    particleBuffer_->Unmap(0, nullptr);
}

void Compute::Draw(ID3D12GraphicsCommandList* commandList) {
    // レンダリングパイプラインステートの設定
    commandList->SetPipelineState(render_->GetPipelineStatee());
    commandList->SetGraphicsRootSignature(render_->GetRootSignature());

    // パーティクルを描画
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

    // パーティクルの数に応じて頂点バッファを設定
    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = particleBuffer_->GetGPUVirtualAddress();
    vbv.StrideInBytes = sizeof(Particle);
    vbv.SizeInBytes = kParticleMax * sizeof(Particle);
    commandList->IASetVertexBuffers(0, 1, &vbv);

    // シェーダーリソースビューの設定
    commandList->SetGraphicsRootDescriptorTable(0, srvGpuHandle_);

    // 描画コール
    commandList->DrawInstanced(kParticleMax, 1, 0, 0);
}

void Compute::InitializeParticle() {
    particles_.resize(kParticleMax);
    const float kWidthRange = 1280.0f;
    const float kHeightRange = 720.0f;
    const float kDepthRange = 100.0f;
    // パーティクルデータの初期化コードをここに追加
    for (int i = 0; i < kParticleMax; ++i) {
        particles_[i].position_.x = static_cast<float>(rand()) / kWidthRange;
        particles_[i].position_.y = static_cast<float>(rand()) / kHeightRange;
        particles_[i].position_.z = static_cast<float>(rand()) / (kDepthRange * 2.0f) - kDepthRange;
        particles_[i].velocity_.x = (static_cast<float>(rand()) / RAND_MAX);
        particles_[i].velocity_.y = (static_cast<float>(rand()) / RAND_MAX);
        particles_[i].velocity_.z = (static_cast<float>(rand()) / RAND_MAX);
        particles_[i].velocity_.Normalize();
    }
}

void Compute::CreateParticleBuffer() {

    // リソースの作成
    D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Particle) * kParticleMax);

    auto hr = sDevice_->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr,
        IID_PPV_ARGS(&particleBuffer_)
    );

    assert(SUCCEEDED(hr));

    Mapping();

    // デスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;  // デスクリプタの数
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;  // デスクリプタヒープのタイプ
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // シェーダから可視
    hr = sDevice_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&srvDescriptorHeap_));
    assert(SUCCEEDED(hr));

    // パーティクルバッファのビューを作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; // フォーマットは不明
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = kParticleMax; // パーティクルの数
    srvDesc.Buffer.StructureByteStride = sizeof(Particle); // パーティクルのデータ構造のサイズ

    srvCpuHandle_ = srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    srvGpuHandle_ = srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();

    sDevice_->CreateShaderResourceView(particleBuffer_.Get(), &srvDesc, srvCpuHandle_);

    // パーティクルバッファをシェーダにバインド
    // これはシェーダプログラムに依存しますが、例えば次のように設定することがあります
    // Compute シェーダの場合
    // commandList->SetComputeRootShaderResourceView(0, particleBufferView_);
}

void Compute::CreateParticleRootSignature() {
    CD3DX12_DESCRIPTOR_RANGE uavDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // 1つのUAVを指定
    CD3DX12_ROOT_PARAMETER rootParameters[1];
    rootParameters[0].InitAsDescriptorTable(1, &uavDescriptorRange, D3D12_SHADER_VISIBILITY_ALL); // シェーダーからアクセス可能
    // 他のルートパラメータを設定

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(1, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);
    ComPtr<ID3DBlob> signatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    sDevice_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&particleRootSignature_));
}

void Compute::CreateParticlePipelineState() {
    // パイプラインステートの設定
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.CS = {
        computeBlob_->GetBufferPointer(),
        computeBlob_->GetBufferSize()
    };
    psoDesc.pRootSignature = particleRootSignature_.Get();

    // パイプラインステートを作成
    sDevice_->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&particlePipelineState_));
}

void Compute::Mapping() {
    Particle* pParticleData;
    particleBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&pParticleData));
    memcpy(pParticleData, particles_.data(), sizeof(Particle) * kParticleMax);
    particleBuffer_->Unmap(0, nullptr);
}

void Compute::ParticleShaderCompile() {
    //Shaderをコンパイルする
    computeBlob_ = DXCCompiler::CompileShader(L"ComputeShader.hlsl",L"cs_6_0");
    assert(computeBlob_ != nullptr);
}
