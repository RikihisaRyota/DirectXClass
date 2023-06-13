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
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("resources/white1x1.png");
	
	// Spriteの静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// Basicの静的初期化
	Basic::StaticInitialize(dxCommon->GetDevice(),WinApp::kWindowWidth, WinApp::kWindowHeight);

	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// ゲームシーンの初期化
	GameScene* gameScene = nullptr;
	gameScene = new GameScene();
	gameScene->Initialize();



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

		// 描画開始
		dxCommon->PreDraw();

		//// ゲームシーンの描画
		gameScene->Draw();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーン解放
	gameScene->Release();
	SafeDelete(gameScene);

	// テクスチャマネージャーの解放
	TextureManager::Release();
	
	// ImGui解放
	imguiManager->Finalize();

	// Sprite解放
	Sprite::Release();

	// Basic解放
	Basic::Release();

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

