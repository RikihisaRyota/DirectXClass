#include "WinApp.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "TextureManager.h"

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
	input = Input::GetInstans();
	input->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

	// BasicGraphicsPiplineの静的初期化
	BasicGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// ToonGraphicsPiplineの静的初期化
	ToonGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// LineGraphicsPiplineの静的初期化
	LineGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// Spriteの静的初期化
	Sprite::SetDevice(dxCommon->GetDevice());

	// Basicの静的初期化
	Basic::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// OBJの静的初期化
	OBJ::SetDevice(dxCommon->GetDevice());

	// Cubeのデバイスセット
	Cube::SetDevice(dxCommon->GetDevice());

	// Sphereのデバイスセット
	Sphere::SetDevice(dxCommon->GetDevice());

	// Material
	Material::SetDevice(dxCommon->GetDevice());

	// Mesh
	Mesh::SetDevice(dxCommon->GetDevice());

	// Model
	Model::SetDevice(dxCommon->GetDevice());

	// Line
	Line::SetDevice(dxCommon->GetDevice());
	Line::GetInstance()->Initialize();

	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// ゲームシーンの初期化
	GameScene* gameScene = nullptr;
	gameScene = new GameScene();
	gameScene->Initialize();

	// Pera
	Pera* pera = nullptr;
	pera = new Pera();
	pera->Initialize(dxCommon, dxCommon->GetBackBuff(), dxCommon->GetRTVDescriptorHeap());

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
		//// ゲームシーンの毎フレーム処理
		gameScene->Update();
		// ImGui受付終了
		imguiManager->End();
		// テクスチャマネージャーの描画準備
		TextureManager::GetInstance()->PreDraw();
		//// SRV->RTV
		//pera->PreDraw();
		//// ゲームシーンの描画
		//gameScene->Draw2();
		//// RTV->SRV
		//pera->PostDraw();
		// 描画開始
		dxCommon->PreDraw();
		// LineのPreDraw
		
		// ゲームシーンの描画
		gameScene->Draw();
		// ImGui描画
		imguiManager->Draw();
		// 描画終わり
		dxCommon->PostDraw();
	}
	SafeDelete(pera);

	// ゲームシーン解放
	gameScene->Release();
	SafeDelete(gameScene);

	// ImGui解放
	imguiManager->Finalize();

	// Line解放
	Line::Release();

	// Basic解放
	Basic::Release();

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

