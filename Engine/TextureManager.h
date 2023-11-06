#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <cstdint>
#include <string>
#include <wrl.h>
#include <array>
#include "DirectXTex.h"


class DirectXCommon;
class TextureManager
{
public: // 静的メンバ関数
	enum class TextureHandle {
		WHITE1x1,
		TOON,
		COUNT,
	};

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
	void CreateShaderResourceView(const DirectX::TexMetadata& metadata, ID3D12Resource* textureResourec, uint32_t count);
	//void CreateShaderResourceView(ID3D12Resource* textureResourec);
	/// <summary>
	/// デスクリプタテーブルをセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParamIndex">ルートパラメータ番号</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetGraphicsRootDescriptorTable(
		ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle);
	
	/// <summary>
	/// デスクリプタテーブルをセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParamIndex">ルートパラメータ番号</param>
	/// <param name="SRV">ID3D12DescriptorHeap</param>
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, ID3D12DescriptorHeap* SRV);

	/// <summary>
	/// システム初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(DirectXCommon* device);

	/// <summary>
	/// 解放
	/// </summary>
	/// 2023/11/6srvDiscripotrをDirextXCommonに移動したため使えない
	void UnLoadInternal();

	/// リソース情報取得
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>リソース情報</returns>
	const D3D12_RESOURCE_DESC GetResoureDesc(uint32_t textureHandle);
private: // メンバ変数
	// デバイス
	static DirectXCommon* device_;
	// テクスチャコンテナ
	std::array<Texture, kNumDescriptors> textures_;
	std::array<bool, kNumDescriptors> useTable_;
	uint32_t textureCount_;
};