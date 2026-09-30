//----------------------------------------------------------------------------
//! @file   ImageButton.cpp
//! @brief  画像ボタンの実装
//! @detail トグルとワンショット入力に対応した画像ボタンの基底クラスの機能を提供します。
//----------------------------------------------------------------------------
#include "ImageButton.h"
#include "UIConstants.h"
#include "DxLib.h"

namespace UI
{
	//-----------------------------------------------------------
	//! @brief デストラクタ
	//-----------------------------------------------------------
	ImageButton::~ImageButton()
	{
		End();
	}

	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void ImageButton::Init()
	{
		image_off_   = 0;
		image_on_    = 0;
		pos_         = { 0.0f, 0.0f };
		size_        = { 0.0f, 0.0f };
		state_       = ButtonState::OFF;
		was_pressed_ = false;
		scale_       = 1.0f;
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void ImageButton::Update()
	{
		int mx = 0;
		int my = 0;

		GetMousePoint(&mx, &my);

		float left   = pos_.x - size_.w * 0.5f;
		float top    = pos_.y - size_.h * 0.5f;
		float right  = left + size_.w;
		float bottom = top + size_.h;

		bool is_hover = (mx >= left && mx <= right && my >= top && my <= bottom);

		scale_ = is_hover ? BUTTON_HOVER_SCALE : 1.0f;

		bool now_pressed = (is_hover && (GetMouseInput() & MOUSE_INPUT_LEFT));

		if (input_type_ == ButtonInputType::TOGGLE)
		{
			if (now_pressed && !was_pressed_)
			{
				state_ = (state_ == ButtonState::ON ? ButtonState::OFF : ButtonState::ON);
			}
		}
		else
		{
			state_ = (now_pressed ? ButtonState::ON : ButtonState::OFF);
		}

		was_pressed_ = now_pressed;
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void ImageButton::Draw()
	{
		int handle = (state_ == ButtonState::ON ? image_on_ : image_off_);

		DrawRotaGraphF(pos_.x, pos_.y, scale_, 0.0f, handle, true);
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void ImageButton::End()
	{
		if (image_off_ != 0)
		{
			DeleteGraph(image_off_);
			image_off_ = 0;
		}

		if (image_on_ != 0)
		{
			DeleteGraph(image_on_);
			image_on_ = 0;
		}
	}

	//-----------------------------------------------------------
	//! @brief 入力タイプ設定
	//-----------------------------------------------------------
	void ImageButton::SetInputType(ButtonInputType type)
	{
		input_type_ = type;
	}

	//-----------------------------------------------------------
	//! @brief ON状態判定
	//-----------------------------------------------------------
	bool ImageButton::IsOn() const
	{
		return state_ == ButtonState::ON;
	}

	//-----------------------------------------------------------
	//! @brief ON状態設定
	//-----------------------------------------------------------
	void ImageButton::SetOn()
	{
		state_ = ButtonState::ON;
	}

	//-----------------------------------------------------------
	//! @brief OFF状態設定
	//-----------------------------------------------------------
	void ImageButton::SetOff()
	{
		state_ = ButtonState::OFF;
	}

	//-----------------------------------------------------------
	//! @brief ボタン画像設定
	//-----------------------------------------------------------
	void ImageButton::SetButtonImage(const char* img_on, const char* img_off, const FloatPos& pos)
	{
		// 既存の画像ハンドルがあれば解放
		if (image_off_ != 0)
		{
			DeleteGraph(image_off_);
			image_off_ = 0;
		}

		if (image_on_ != 0)
		{
			DeleteGraph(image_on_);
			image_on_ = 0;
		}

		image_off_ = LoadGraph(img_off);
		image_on_  = LoadGraph(img_on);
		pos_       = pos;

		// OFF画像を基準にサイズ取得
		int w = 0;
		int h = 0;

		GetGraphSize(image_off_, &w, &h);

		size_.w = static_cast<float>(w);
		size_.h = static_cast<float>(h);
	}

	//-----------------------------------------------------------
	//! @brief 前クリック状態リセット
	//-----------------------------------------------------------
	void ImageButton::ResetClick()
	{
		was_pressed_ = false;
	}
}
