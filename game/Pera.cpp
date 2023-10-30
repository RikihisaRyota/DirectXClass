#include "Pera.h"

#include <cassert>

#include "DirectXCommon.h"
#include "TextureManager.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
DirectXCommon* Pera::sDxCommon_ = nullptr;

void Pera::Initialize(DirectXCommon* dxCommon, ID3D12Resource* buff, ID3D12DescriptorHeap* rtv) {
	HRESULT hr = S_FALSE;
	sDxCommon_ = dxCommon;
	// マルチパスレンダリングの設定
	{
		auto resDesc = buff->GetDesc();

		D3D12_HEAP_PROPERTIES heapProp =
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		// レンダリング時のクリア値と同じ
		float clearColor[] = { 0.0f,0.0f,0.0f,1.0f };
		D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);

		hr = sDxCommon_->GetInstance()->GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&peraResource_)
		);
		//ディスクリプタのハンドルを取得する
		rtvHandle_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtv->GetCPUDescriptorHandleForHeapStart(),
			2,
			sDxCommon_->GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		// レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2Dテクスチャとして書き込む
		// レンダーターゲットビューの生成
		sDxCommon_->GetInstance()->GetDevice()->CreateRenderTargetView(peraResource_.Get(), &rtvDesc, rtvHandle_);

		TextureManager::GetInstance()->CreateShaderResourceView(peraResource_.Get());
	}
}

void Pera::PreDraw() {
	// リソースバリアを変更(シェーダーリソース→レンダーターゲット)
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		peraResource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	sDxCommon_->GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

	// デプスステンシルビューのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = sDxCommon_->GetInstance()->GetDSVHandle();

	// レンダーターゲットとデプスステンシルをOMに設定
	sDxCommon_->GetInstance()->GetCommandList()->OMSetRenderTargets(1, &rtvHandle_, false, &dsvHandle);

	// 全画面クリア
	ClearRenderTarget();

	// ビューポートの設定
	CD3DX12_VIEWPORT viewport =
		CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight);
	sDxCommon_->GetInstance()->GetCommandList()->RSSetViewports(1, &viewport);
	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight);
	sDxCommon_->GetInstance()->GetCommandList()->RSSetScissorRects(1, &rect);
}

void Pera::PostDraw() {
	// リソースバリアの変更(レンダーターゲット→シェーダーリソース)
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        peraResource_.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    sDxCommon_->GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);
}

void Pera::ClearRenderTarget() {
	// 全画面のクリア
	float clearColor[] = { 0.0f,0.0f,0.0f,1.0f };
	sDxCommon_->GetInstance()->GetCommandList()->ClearRenderTargetView(rtvHandle_, clearColor, 0, nullptr);
}
