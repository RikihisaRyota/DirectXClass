#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <cstdint>
#include <string>
#include <wrl.h>
#include <array>
#include "DirectXTex.h"


class TextureManager
{
public: // 静的メンバ関数

	/// <summary>
	/// テクスチャ
	/// </summary>
	struct Texture {
		// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
		// 名前
		std::string name;
	};

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>テクスチャハンドル</returns>
	static uint32_t Load(const std::string& fileName);

	/// <summary>
	/// 解放
	/// </summary>
	static void Release();

public: // 静的メンバ変数
	// デスクリプターの数
	static const size_t kNumDescriptors = 256;

public: // メンバ関数

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	uint32_t LoadInternal(const std::string& filePath);

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>テクスチャハンドル</returns>
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	/// <summary>
	/// TextureResourceを作る
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="metadata">TexMetadata</param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResourec(const DirectX::TexMetadata& metadata);

	/// <summary>
	/// TextureResourceにデータを転送
	/// </summary>
	/// <param name="texture">Texture</param>
	/// <param name="mipImages">mipImages</param>
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	/// <summary>
	/// srvの生成
	/// </summary>
	/// <param name="metadata">metadata</param>
	void CreateShaderResourceView(const DirectX::TexMetadata& metadata, ID3D12Resource* texture);

	/// <summary>
	/// デスクリプタテーブルをセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParamIndex">ルートパラメータ番号</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetGraphicsRootDescriptorTable(
		ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle);

	/// <summary>
	/// システム初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 解放
	/// </summary>
	void UnLoadInternal();

	/// <summary>
	/// GetCPUDescriptorHandle
	/// </summary>
	/// <param name="descriptorHeap">descriptorHeap</param>
	/// <param name="descriptorSize">descriptorSize</param>
	/// <param name="index">index</param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// GetCPUDescriptorHandle
	/// </summary>
	/// <param name="descriptorHeap">descriptorHeap</param>
	/// <param name="descriptorSize">descriptorSize</param>
	/// <param name="index">index</param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

private: // メンバ変数
	// デバイス
	ID3D12Device* device_;
	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize = 0u;
	// デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	// テクスチャコンテナ
	std::array<Texture, kNumDescriptors> textures_;
	std::array<bool, kNumDescriptors> useTable_;
	// カウント
	uint32_t kNumDescriptorsCount = 0u;
};