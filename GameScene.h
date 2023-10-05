#pragma once
#include <cstdlib>
#include <memory>

#include "Audio.h"
#include "CubeRenderer.h"
#include "DebugCamera.h"
#include "Input.h"
#include "PrimitiveDrawer.h"
#include "Model.h"
#include "OBJ.h"

#include "Pera.h"

#include "SafeDelete.h"
#include "PlaneRenderer.h"
#include "SphereRenderer.h"
#include "Sprite.h"

class DirectXCommon;
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 解放
	/// </summary>
	void Release();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;
	// ライト
	cDirectionalLight* directionalLight_ = nullptr;

	uint32_t textureHandle_ = 0u;

	uint32_t peraTextureHandle_=0u;

	struct UVtranslation {
		Vector3 scale_;
		Vector3 rotate_;
		Vector3 translate_;
	};
	
	std::unique_ptr<Sprite> sprite_[2];

	/// <summary>
	/// ゲーム用
	/// </summary>
	ViewProjection viewProjection_;
	// obj
	std::vector<std::unique_ptr<OBJ>> obj_;
	std::vector<std::unique_ptr<WorldTransform>> objWorldTransform_;
	std::vector<uint32_t> objUseTexture_;
	std::vector<uint32_t> objUseToon_;
	std::vector< std::unique_ptr<cMaterial>> objMaterial_;
	std::vector< std::unique_ptr<UVtranslation>> objUVtranslation_;
	// teapot
	std::vector<std::unique_ptr<OBJ>> teapot_;
	std::vector<std::unique_ptr<WorldTransform>> teapotWorldTransform_;
	std::vector<uint32_t> teapotUseTexture_;
	std::vector<uint32_t> teapotUseToon_;
	std::vector< std::unique_ptr<cMaterial>> teapotMaterial_;
	std::vector< std::unique_ptr<UVtranslation>> teapotUVtranslation_;
	// bunny
	std::vector<std::unique_ptr<OBJ>> bunny_;
	std::vector<std::unique_ptr<WorldTransform>> bunnyWorldTransform_;
	std::vector<uint32_t> bunnyUseTexture_;
	std::vector<uint32_t> bunnyUseToon_;
	std::vector< std::unique_ptr<cMaterial>> bunnyMaterial_;
	std::vector< std::unique_ptr<UVtranslation>> bunnyUVtranslation_;
	// multiMesh
	std::vector<std::unique_ptr<OBJ>> multiMesh_;
	std::vector<std::unique_ptr<WorldTransform>> multiMeshWorldTransform_;
	std::vector<uint32_t> multiMeshUseTexture_;
	std::vector<uint32_t> multiMeshUseToon_;
	std::vector< std::unique_ptr<cMaterial>>multiMeshMaterial_;
	std::vector< std::unique_ptr<UVtranslation>> multiMeshUVtranslation_;
	// cube
	std::vector<std::unique_ptr<CubeRenderer>> cube_;
	std::vector<std::unique_ptr<WorldTransform>> cubeWorldTransform_;
	std::vector<uint32_t> cubeUseTexture_;
	std::vector<uint32_t> cubeUseToon_;
	std::vector< std::unique_ptr<cMaterial>> cubeMaterial_;
	std::vector< std::unique_ptr<UVtranslation>> cubeUVtranslation_;
	// sphere
	std::vector<std::unique_ptr<SphereRenderer>> sphere_;
	std::vector<std::unique_ptr<WorldTransform>> sphereWorldTransform_;
	std::vector<uint32_t> sphereUseTexture_;
	std::vector<uint32_t> sphereUseToon_;
	std::vector< std::unique_ptr<cMaterial>> sphereMaterial_;
	std::vector< std::unique_ptr<UVtranslation>> sphereUVtranslation_;
	// plane
	std::vector<std::unique_ptr<PlaneRenderer>> plane_;
	std::vector<std::unique_ptr<WorldTransform>> planeWorldTransform_;
	std::vector<uint32_t> planeUseTexture_;
	std::vector<uint32_t> planeUseToon_;
	std::vector<std::unique_ptr<cMaterial>> planeMaterial_;
	std::vector<std::unique_ptr<UVtranslation>> planeUVtranslation_;

	// multiMaterial
	std::vector<std::unique_ptr<Model>> multiMaterial_;
	std::vector<std::unique_ptr<WorldTransform>> multiMaterialWorldTransform_;
	std::vector<uint32_t> multiMaterialUseTexture_;
	std::vector<uint32_t> multiMaterialUseToon_;
	std::vector<std::vector<std::unique_ptr<cMaterial>>> multiMaterialMaterial_;
	std::vector<std::vector<std::unique_ptr<UVtranslation>>> multiMaterialUVtranslation_;

	// suzanne
	std::vector<std::unique_ptr<Model>> suzanne_;
	std::vector<std::unique_ptr<WorldTransform>> suzanneWorldTransform_;
	std::vector<uint32_t> suzanneUseTexture_;
	std::vector<uint32_t> suzanneUseToon_;
	std::vector<std::vector<std::unique_ptr<cMaterial>>> suzanneMaterial_;
	std::vector<std::vector<std::unique_ptr<UVtranslation>>> suzanneUVtranslation_;

	// fence
	std::vector<std::unique_ptr<Model>> fence_;
	std::vector<std::unique_ptr<WorldTransform>> fenceWorldTransform_;
	std::vector<uint32_t> fenceUseTexture_;
	std::vector<uint32_t> fenceUseToon_;
	std::vector<std::vector<std::unique_ptr<cMaterial>>> fenceMaterial_;
	std::vector<std::vector<std::unique_ptr<UVtranslation>>> fenceUVtranslation_;
	// 音声再生
	Audio* audio_;

	size_t SESoundHandle_;

	size_t titleLoopSoundHandle_;
	size_t titleLoopPlayHandle_;
	size_t inGameLoopSoundHandle_;
	size_t inGameLoopPlayHandle_;
	bool isTitle_;
};