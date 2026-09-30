//----------------------------------------------------------------------------
//! @file   DxMain.cpp
//! @brief  メインエントリーポイント
//! @detail DXライブラリの初期化、ゲームループ、終了処理を行います。
//----------------------------------------------------------------------------
#include "DxLib.h"
#include "Engine.h"
#include "GameConstants.h"
#include "GameScene.h"
#include "InputManager.h"
#include "Font.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//--------------------------------------------------------
	// DXライブラリ初期化
	//--------------------------------------------------------
	SetOutApplicationLogValidFlag(FALSE);   //!< ログ出力をしない
	SetAlwaysRunFlag(TRUE);// ウィンドウが非アクティブな状態でも処理を続行するように設定
	ChangeWindowMode(TRUE);// ウィンドウモード設定
	SetBackgroundColor(WINDOW_BG_COLOR_R, WINDOW_BG_COLOR_G, WINDOW_BG_COLOR_B);// ウィンドウ背景色設定
	SetGraphMode(SCREEN_W, SCREEN_H, 32);// 画面モード設定

	if (DxLib_Init() == -1)
		return -1;

	//--------------------------------------------------------
	// ウィンドウ設定
	//--------------------------------------------------------
	SetDragFileValidFlag(TRUE);// ファイルのドラッグ＆ドロップ有効化
	SetMainWindowText("VirelleaNotesEngine");// ウィンドウタイトル設定

	//--------------------------------------------------------
	// フォント初期化
	//--------------------------------------------------------
	Font::FontInit();

	//--------------------------------------------------------
	// ゲームシーン作成と初期化
	//--------------------------------------------------------
	auto engine_scene = std::make_unique<Scene::GameScene>();

	engine_scene->Init();

	//--------------------------------------------------------
	// メインループ
	//--------------------------------------------------------
	while (ProcessMessage() == 0 && !InputManager::InputManager::PushHitKey(KEY_INPUT_ESCAPE))
	{
		ClearDrawScreen();

		InputManager::InputManager::Update();

		engine_scene->Update();
		engine_scene->Draw();

		ScreenFlip();
	}

	//--------------------------------------------------------
	// 終了処理
	//--------------------------------------------------------
	engine_scene->End();
	Font::FontExit();
	DxLib_End();

	return 0;
}
