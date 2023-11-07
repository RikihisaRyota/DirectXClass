#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

class DirectXCommon;

class ImGuiManager
{
public:
	static ImGuiManager* GetInstance();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// DirectX基盤インスタンス（借りてくる）
	DirectXCommon* dxCommon_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle_;
private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;
};

