#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <map>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "Matrix4x4.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

#pragma comment(lib,"dxcompiler.lib")

class Basic
{
public: // サブクラス
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexPos {
		Vector4 pos; // xyz座標
		Vector3 normal; // 法線
		Vector2 texcoord; // uv座標
	};
	/// <summary>
	/// マテリアル
	/// </summary>
	struct Material {
		Vector4 color; // 色
		int32_t enableLightint;
	};
	/// <summary>
	/// ライト
	/// </summary>
	struct DirectionalLight {
		Vector4 color; //!< ライトの色
		Vector3 direction; //!< ライトの色
		float intensiy; //!< 輝度
	};
	struct Culling {
		int32_t flag; //!< true->表
	};
private:
	enum ROOT_PARAMETER_TYP {
		WORLDTRANSFORM,
		VIEWPROJECTION,
		CULLINGFLUG,
		MATERIAL,
		LIGHTING,
		TEXTURE,
		TOONTEXTURE,
		COUNT,
	};
public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(ID3D12Device* device, int window_width, int window_height);

	/// <summary>
	/// newとInitialize
	/// </summary>
	/// <param name="flag">カリング:true->表,false->裏</param>
	/// <returns>生成</returns>
	static Basic* Create(bool flag = true);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 解放
	/// </summary>
	static void Release();
	
private: // 静的メンバ変数
	/// <summary>
	/// グラフィックパイプラインの生成
	/// </summary>
	/// <param name="flag">カリング:true->表,false->裏</param>
	void InitializeGraphicsPipeline(bool flag);

	/// <summary>
	/// DXCの初期化
	/// </summary>
	static void dxcCompilerInitialize();

	/// <summary>
	/// CompileShader関数
	/// </summary>
	static IDxcBlob* CompileShader(
		//ConpilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したのもを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);
private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* sDevice;
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> sCommandList;
	// dxcCompierを初期化
	static Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
	// カリングフラグ
	static bool cullingFlag_;
public: // メンバ関数
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(
		const WorldTransform& worldTransform, const ViewProjection& viewProjection,
		uint32_t textureHadle = static_cast<int>(TextureManager::TextureHandle::WHITE1x1));
public:
	void SetMaterial(const Vector4& color) { material_->color = color; }
	void SetLighting(const Vector4& color, const Vector3& direction, float intensiy) {
		directionalLight_->color = color,
			directionalLight_->direction = direction,
			directionalLight_->intensiy = intensiy;
	}
	void SetIsLighting(bool flag) { material_->enableLightint = flag; }
	bool GetIsLighting() { return material_->enableLightint; }

	Vector4 GetLightColor() { return directionalLight_->color; }
	Vector3 GetLightDirection() { return directionalLight_->direction; }
	float GetLightIntensiy() { return directionalLight_->intensiy; }
private:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// データ生成
	/// </summary>
	void CreateBasic();
private: // メンバ変数
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff_;
	// ライティングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightBuff_;
	// カリングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cullingBuff_;
	// 頂点データ配列
	std::vector<VertexPos> vertices_;
	// 頂点インデックスデータ
	std::vector<uint16_t> indices_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// マテリアル
	Material* material_;
	// ライティング
	DirectionalLight* directionalLight_;
	// カリング
	Culling* culling_;
};

