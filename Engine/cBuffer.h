#pragma once

#include <d3d12.h>
#include <d3dx12.h>

#include <memory>


struct Buffer {
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle{};

	D3D12_CPU_DESCRIPTOR_HANDLE dpsCPUHandle{};
};