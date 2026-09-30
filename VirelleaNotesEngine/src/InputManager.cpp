//----------------------------------------------------------------------------
//! @file   InputManager.cpp
//! @brief  入力管理の実装
//! @detail 全キー・マウス入力の状態取得およびフレーム管理を行う機能を提供します。
//----------------------------------------------------------------------------
#include "InputManager.h"
#include <cstring>

namespace InputManager
{
	// 静的メンバ変数定義
	char InputManager::key_buffer_[256] = {};
	int InputManager::key_frame_[256] = {};
	int InputManager::mouse_left_frame_ = 0;
	int InputManager::mouse_right_frame_ = 0;
	int InputManager::mouse_wheel_rot_ = 0;

	//-----------------------------------------------------------
	//! @brief 入力状態更新
	//-----------------------------------------------------------
	void InputManager::Update()
	{
		// 全キーの押下状態を取得
		GetHitKeyStateAll(key_buffer_);

		// 各キーのフレーム更新
		for (int i = 0; i < 256; i++)
		{
			key_frame_[i] = key_buffer_[i] ? key_frame_[i] + 1 : 0;
		}

		// マウスボタンフレーム更新
		mouse_left_frame_  = (GetMouseInput() & MOUSE_INPUT_LEFT) ? mouse_left_frame_ + 1 : 0;
		mouse_right_frame_ = (GetMouseInput() & MOUSE_INPUT_RIGHT) ? mouse_right_frame_ + 1 : 0;

		// マウスホイール更新
		mouse_wheel_rot_ = GetMouseWheelRotVol();
	}

	//-----------------------------------------------------------
	//! @brief キーが押された瞬間判定
	//-----------------------------------------------------------
	bool InputManager::PushHitKey(int key)
	{
		return key_frame_[key] == 1;
	}

	//-----------------------------------------------------------
	//! @brief マウスボタン押下中判定
	//-----------------------------------------------------------
	bool InputManager::CheckMouseInput(int button)
	{
		return (GetMouseInput() & button) != 0;
	}

	//-----------------------------------------------------------
	//! @brief マウスボタン押下瞬間判定
	//-----------------------------------------------------------
	bool InputManager::PushMouseInput(int button)
	{
		if ((button & MOUSE_INPUT_LEFT) && mouse_left_frame_ == 1) return true;
		if ((button & MOUSE_INPUT_RIGHT) && mouse_right_frame_ == 1) return true;

		return false;
	}

	//-----------------------------------------------------------
	//! @brief マウスホイール回転量取得
	//-----------------------------------------------------------
	int InputManager::GetMouseWheel()
	{
		return mouse_wheel_rot_;
	}

	//-----------------------------------------------------------
	//! @brief マウスX座標取得
	//-----------------------------------------------------------
	int InputManager::GetMouseX()
	{
		int x = 0;
		int y = 0;

		GetMousePoint(&x, &y);

		return x;
	}

	//-----------------------------------------------------------
	//! @brief マウスY座標取得
	//-----------------------------------------------------------
	int InputManager::GetMouseY()
	{
		int x = 0;
		int y = 0;

		GetMousePoint(&x, &y);

		return y;
	}
}
