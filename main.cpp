#include "WinApp.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "SceneManager.h"

#include <dxgidebug.h>
#include <cassert>

#pragma comment(lib,"dxguid.lib")


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ゲームウィンドウの作成
	WinApp* win = nullptr;
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"DirectXClass");

	// DirectXの初期化処理
	DirectXCommon* dxCommon = nullptr;
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	// 入力の初期化
	Input* input = nullptr;
	input = Input::GetInstance();
	input->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(),WinApp::kWindowWidth,WinApp::kWindowHeight);

	// BasicGraphicsPiplineの静的初期化
	BasicGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// ToonGraphicsPiplineの静的初期化
	ToonGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// LineGraphicsPiplineの静的初期化
	LineGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// Spriteの静的初期化
	Plate::SetDevice(dxCommon->GetDevice());

	// OBJの静的初期化
	OBJ::SetDevice(dxCommon->GetDevice());

	// Cubeのデバイスセット
	Cube::SetDevice(dxCommon->GetDevice());

	// Sphereのデバイスセット
	ModelSphere::SetDevice(dxCommon->GetDevice());

	// Material
	Material::SetDevice(dxCommon->GetDevice());

	// Mesh
	Mesh::SetDevice(dxCommon->GetDevice());

	// Model
	Model::SetDevice(dxCommon->GetDevice());

	// Line
	PrimitiveDrawer::SetDevice(dxCommon->GetDevice());
	PrimitiveDrawer::GetInstance()->Initialize();

	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

	// ゲームシーンの初期化
	SceneManager* sceneManager = nullptr;
	sceneManager = new SceneManager(static_cast<uint32_t>(TextureManager::TextureHandle::WHITE1x1));

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}
		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		// ゲームシーンの毎フレーム処理
		sceneManager->Update();
		// ImGui受付終了
		imguiManager->End();
		// テクスチャマネージャーの描画準備
		TextureManager::GetInstance()->PreDraw();
		// 描画開始
		dxCommon->PreDraw();		
		// ゲームシーンの描画
		sceneManager->Draw();
		// ImGui描画
		imguiManager->Draw();
		// 描画終わり
		dxCommon->PostDraw();
	}

	// ゲームシーン解放
	SafeDelete(sceneManager);

	// ImGui解放
	imguiManager->Finalize();

	// Line解放
	PrimitiveDrawer::Release();

	Sprite::Release();

	// テクスチャマネージャーの解放
	TextureManager::Release();

	// DirectXの解放
	dxCommon->Release();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	// リリースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
	return 0;
}

