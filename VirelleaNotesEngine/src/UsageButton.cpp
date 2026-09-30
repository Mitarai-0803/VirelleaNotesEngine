//----------------------------------------------------------------------------
//! @file   UsageButton.cpp
//! @brief  使い方ボタンの実装
//! @detail クリックで使い方を表示するボタンの機能を提供します。
//----------------------------------------------------------------------------
#include <DxLib.h>
#include "UsageButton.h"
#include "GameConstants.h"
#include "InputManager.h"
#include <string>
#include <windows.h>
#include <filesystem>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void UsageButton::OnInit()
	{
		// ボタン画像と座標を設定
		Init();
		SetInputType(UI::ButtonInputType::TOGGLE);
		SetOff();

		// ボタン画像設定
		SetButtonImage(USAGE_BUTTON_DOWN_PATH, USAGE_BUTTON_PATH, { USAGE_DELETE_BTN_X, USAGE_DELETE_BTN_Y });

		char usage_page_path[200];

		// 使い方画像の読み込み
		for (int i = 0; i < USAGE_PAGE_MAX; ++i)
		{
			sprintf_s(usage_page_path, USAGE_PAGE_PATH, (i + 1));

			use_image_handle_[i] = LoadGraph(usage_page_path);
		}
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void UsageButton::OnUpdate()
	{
		// ボタン更新
		Update();

		if (IsOn())
		{
			// マウスホイールでページ変更
			int current_wheel = InputManager::InputManager::GetMouseWheel();
			int diff = current_wheel - previous_wheel_value_;

			if (diff > 0)
			{
				current_usage_page_++;
			}
			else if (diff < 0)
			{
				current_usage_page_--;
			}

			previous_wheel_value_ = current_wheel;

			// ページ範囲の制限
			if (current_usage_page_ < 0)
			{
				current_usage_page_ = 0;
			}
			else if (current_usage_page_ >= USAGE_PAGE_MAX)
			{
				current_usage_page_ = USAGE_PAGE_MAX - 1;
			}
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void UsageButton::OnDraw()
	{
		// ボタン描画
		Draw();

		// 使い方を表示するなら
		if (IsOn())
		{
			// 使い方画像の描画
			DrawRotaGraph(HALF_SCREEN_W, HALF_SCREEN_H, 1.0f, 0.0f, use_image_handle_[current_usage_page_], true);
		}
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void UsageButton::OnEnd()
	{
		End();

		// 使い方画像の解放
		for (int i = 0; i < USAGE_PAGE_MAX; ++i)
		{
			DeleteGraph(use_image_handle_[i]);
		}
	}
}