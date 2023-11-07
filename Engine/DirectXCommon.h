#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>

#include <memory>

#include "PostEffectPipeline.h"
#include "WinApp.h"


class DirectXCommon {
public:// 静的メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectXCommon* GetInstance();

private:
	struct Buffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer{};

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
		
		D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle{};

		D3D12_CPU_DESCRIPTOR_HANDLE dpsCPUHandle{};
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp, int32_t backBufferWidth = WinApp::kWindowWidth,
		int32_t backBufferHeight = WinApp::kWindowHeight);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget(
	);

	/// <summary>
	/// 深度バッファのクリア
	 /// </summary>
	void ClearDepthBuffer();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return device_.Get(); }

	/// <summary>
	/// コマンドリストの取得
	/// </summary>
	/// <returns>コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	/// <summary>
	/// バックバッファの数
	/// </summary>
	/// <returns>バックバッファ</returns>
	size_t GetBackBufferCount()const { return backBuffers_.size(); }

	/// <summary>
	/// RTV
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetRTVDescriptorHeap() { return rtvDescriptorHeap_.Get(); }
	ID3D12DescriptorHeap* GetSRVDescriptorHeap() { return srvDescriptorHeap_.Get(); }

	/// <summary>
	/// DSV
	/// </summary>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() { return depthBuffer_->dpsCPUHandle; }
	/// <summary>
	/// リリース
	/// </summary>
	void Release();
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle();
	void GetSRVCPUGPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpu, D3D12_GPU_DESCRIPTOR_HANDLE& gpu);
	uint32_t GetSRVDescriptorsCount() {return numSRVDescriptorsCount;}
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle();
	uint32_t GetRTVDescriptorsCount() { return numRTVDescriptorsCount; }
	
private:// メンバ関数

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateRenderTargets();

	/// <summary>
	///  深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();
	/// <summary>
	/// ポストエフェクト初期化
	/// </summary>
	void PostEffectInitialize();
private: // メンバ関数
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

	void WaitForGPU();

	/// <summary>
	/// CreateDepthStencilTexture関数
	/// </summary>
	/// <param name="device"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

	/// <summary>
	/// CreateDescriptorHeap
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
private:// メンバ変数
	// デスクリプターの数
	static const size_t kNumDescriptors = 256;
	// ウィンドウズアプリケーション管理
	WinApp* winApp_;

	// Direct3D関連
	// DXGI関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// コマンド関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	// スワップチェーン関連
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	// レンダーターゲット関連
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	std::vector<Buffer*> backBuffers_;
	uint32_t numRTVDescriptorsCount = 0u;
	// いったん描画する
	Buffer* temporaryBuffer_;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint16_t> indices_;
	// デスクリプタサイズ
	uint32_t numSRVDescriptorsCount = 0u;
	UINT SRVDescriptorHandleIncrementSize = 0u;
	UINT RTVDescriptorHandleIncrementSize = 0u;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	PostEffectGraphicsPipeline* postEffectPipeline_;
	// 深度バッファ関連
	Buffer* depthBuffer_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	// 描画関連
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	UINT64 fenceValue_ = 0;
};