#pragma once

#include <d3dx12.h>
#include <vector>

#include "SpriteGraphicsPipline.h"
#include "Matrix4x4.h"

class Sprite {
public:
public: // サブクラス
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexPosUv {
		Vector4 pos_; // xyz座標
		Vector2 uv_;  // uv座標
	};

	/// <summary>
	/// 定数バッファ用データ構造体
	/// </summary>
	struct ConstBufferData {
		Vector4 color; // 色 (RGBA)
		Matrix4x4 mat;   // ３Ｄ変換行列
	};
public:
	static void SetDevice(ID3D12Device* device);
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	static void PostDraw();
	static Sprite* Create(
		uint32_t textureHandle, const Vector2& position, const Vector4& color = { 1, 1, 1, 1 },
		const Vector2& anchorpoint = { 0.0f, 0.0f }, bool isFlipX = false, bool isFlipY = false);
	void Draw();

	/// <summary>
	/// テクスチャハンドルの設定
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetTextureHandle(uint32_t textureHandle);

	uint32_t GetTextureHandle() { return textureHandle_; }

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const Vector2& position);

	const Vector2& GetPosition() { return position_; }

	/// <summary>
	/// 角度の設定
	/// </summary>
	/// <param name="rotation">角度</param>
	void SetRotation(float rotation);

	float GetRotation() { return rotation_; }

	/// <summary>
	/// サイズの設定
	/// </summary>
	/// <param name="size">サイズ</param>
	void SetSize(const Vector2& size);

	const Vector2& GetSize() { return size_; }

	/// <summary>
	/// アンカーポイントの設定
	/// </summary>
	/// <param name="anchorpoint">アンカーポイント</param>
	void SetAnchorPoint(const Vector2& anchorpoint);

	const Vector2& GetAnchorPoint() { return anchorPoint_; }

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color) { color_ = color; };

	const Vector4& GetColor() { return color_; }

	/// <summary>
	/// 左右反転の設定
	/// </summary>
	/// <param name="isFlipX">左右反転</param>
	void SetIsFlipX(bool isFlipX);

	bool GetIsFlipX() { return isFlipX_; }

	/// <summary>
	/// 上下反転の設定
	/// </summary>
	/// <param name="isFlipX">上下反転</param>
	void SetIsFlipY(bool isFlipY);

	bool GetIsFlipY() { return isFlipY_; }

	/// <summary>
	/// テクスチャ範囲設定
	/// </summary>
	/// <param name="texBase">テクスチャ左上座標</param>
	/// <param name="texSize">テクスチャサイズ</param>
	void SetTextureRect(const Vector2& texBase, const Vector2& texSize);
private:
	void Initialize(uint32_t textureHandle, const Vector2& position,const Vector2& size, const Vector4& color,
		const Vector2& anchorpoint, bool isFlipX , bool isFlipY);
	void BasicDraw();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
	void TransferVertices();
private:
#pragma region DirectX関連
	// デバイス
	static ID3D12Device* sDevice;
	// コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// 射影行列
	static Matrix4x4 sMatProjection;
	// グラフィックパイプライン
	std::unique_ptr<SpriteGraphicsPipline> spriteGraphicsPipline_ = nullptr;
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	VertexPosUv* vertMap_ = nullptr;
	std::vector<VertexPosUv> vertices_;
#pragma endregion
#pragma region インデックスバッファ
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// 頂点インデックスデータ
	std::vector<uint16_t> indices_;
#pragma endregion
#pragma region マテリアル
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff_;
	// 定数バッファマップ
	ConstBufferData* constMap = nullptr;
#pragma endregion
	// Z軸回りの回転角
	float rotation_ = 0.0f;
	// 座標
	Vector2 position_{};
	// スプライト幅、高さ
	Vector2 size_ = { 100.0f, 100.0f };
	// アンカーポイント
	Vector2 anchorPoint_ = { 0, 0 };
	// ワールド行列
	Matrix4x4 matWorld_{};
	// 色
	Vector4 color_ = { 1, 1, 1, 1 };
	// 左右反転
	bool isFlipX_ = false;
	// 上下反転
	bool isFlipY_ = false;
	// テクスチャ始点
	Vector2 texBase_ = { 0, 0 };
	// テクスチャ幅、高さ
	Vector2 texSize_ = { 100.0f, 100.0f };
	// テクスチャハンドル
	uint32_t textureHandle_;
	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc_;
};