#include "WinApp.h"
#include "DirectXCommon.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	WinApp* win=nullptr;
	DirectXCommon* dxCommon = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"DirectXClass");

	// DirectXの初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);


	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}
		// 描画開始
		dxCommon->PreDraw();


		// 描画終了
		dxCommon->PostDraw();
	}
	return 0;
}