#pragma once
#include <d3d12.h>
#include <d3d11.h>
#include <dxcapi.h>
#include <memory>
#include <string>
#include <wrl.h>
#include <vector>

#include "BasicGraphicsPipline.h"
#include "Vector4.h"
#include "Vector3.h"

#pragma comment(lib,"dxcompiler.lib")
class Compute {
private:
	struct Particle {
		Vector4 position_;
		Vector3 velocity_;
		/*uint32_t time_;
		bool isAlive_;*/
	};
public:
	Compute();
	~Compute();
	
	static void SetDevice(ID3D12Device* device);
	
	void Initialize();
	void Update(ID3D12GraphicsCommandList* commandList);
	void Draw(ID3D12GraphicsCommandList* commandList);
private:
	void InitializeParticle();
	void CreateParticleBuffer();
	void CreateParticleRootSignature();
	void ParticleShaderCompile();
	void CreateParticlePipelineState();
	void Mapping();
private:
	static ID3D12Device* sDevice_;
	static const UINT kParticleMax = 100;
	static const UINT  kThreadsPerGroup = 256;
	std::vector<Particle> particles_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> particleRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> particlePipelineState_;
	std::unique_ptr<BasicGraphicsPipline> render_;

	Microsoft::WRL::ComPtr<ID3D12Resource> particleBuffer_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	Microsoft::WRL::ComPtr<IDxcBlob> computeBlob_;
	D3D12_SHADER_RESOURCE_VIEW_DESC particleBufferView_;
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_;

};

