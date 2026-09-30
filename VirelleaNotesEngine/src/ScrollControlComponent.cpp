//----------------------------------------------------------------------------
//! @file   ScrollControlComponent.cpp
//! @brief  スクロール制御コンポーネントの実装
//! @detail コンテンツのスクロール位置とスクロールバーの表示・ドラッグ操作を管理します。
//----------------------------------------------------------------------------
#include "ScrollControlComponent.h"
#include "UIConstants.h"
#include "InputManager.h"
#include "Colors.h"
#include "MyLibraryFunctions.h"

namespace System
{
	//-----------------------------------------------------------
	//! @brief コンストラクタ
	//-----------------------------------------------------------
	ScrollControlComponent::ScrollControlComponent()
		: scroll_(0.0f)
		, max_scroll_(0.0f)
		, view_height_(0.0f)
		, max_scroll_range_(0.0f)
		, scroll_speed_(10.0f)
		, zoom_(1.0f)
		, bar_x_(0)
		, bar_y_(0)
		, bar_width_(SCROLL_BAR_WIDTH)
		, bar_height_(SCROLL_BAR_MIN_HEIGHT)
	{
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void ScrollControlComponent::Update()
	{
		// マウスホイールの入力を取得
		wheel_ = InputManager::InputManager::GetMouseWheel();

		// ホイールが回転していればスクロール値を変更
		if (wheel_ != 0)
		{
			scroll_ -= static_cast<float>(wheel_) * scroll_speed_;
		}

		// スクロール値を範囲内に制限（0〜最大範囲）
		if (scroll_ < 0.0f)
		{
			scroll_ = 0.0f;
		}

		if (scroll_ > max_scroll_range_)
		{
			scroll_ = max_scroll_range_;
		}

		// バーのサイズと位置を計算
		UpdateScrollRange();

		// マウス座標を取得
		int mx = 0;
		int my = 0;

		GetMousePoint(&mx, &my);

		// 左クリックの押下状態を取得
		bool leftPressed = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

		// バーを掴んだ瞬間の判定
		if (MyLibrary::CheckPointBoxHit(static_cast<float>(mx), static_cast<float>(my),
		                                static_cast<float>(bar_x_), static_cast<float>(bar_y_),
		                                static_cast<float>(bar_width_), static_cast<float>(bar_height_)) && leftPressed && !dragging_)
		{
			dragging_   = true;
			dragOffset_ = my - bar_y_;
		}

		// ドラッグ中の処理
		if (dragging_)
		{
			if (leftPressed)
			{
				// マウスY座標からバーY座標を計算（オフセット考慮）
				bar_y_ = my - dragOffset_;

				// バー位置を表示範囲内に制限
				if (bar_y_ < 0) bar_y_ = 0;
				if (bar_y_ > static_cast<int>(view_height_) - bar_height_) bar_y_ = static_cast<int>(view_height_) - bar_height_;

				// バー位置からスクロール値に変換
				float denom = view_height_ - static_cast<float>(bar_height_);
				if (denom != 0.0f)
				{
					scroll_ = (static_cast<float>(bar_y_) / denom) * max_scroll_range_;
				}
			}
			else
			{
				dragging_ = false;
			}
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void ScrollControlComponent::Draw()
	{
		// 最大スクロール範囲がある場合、バーY座標をスクロール値から計算
		if (max_scroll_range_ > 0.0f)
		{
			float denom = view_height_ - static_cast<float>(bar_height_);
			float bar_y_f = 0.0f;

			if (denom != 0.0f)
			{
				bar_y_f = (scroll_ / max_scroll_range_) * denom;
			}

			bar_y_ = static_cast<int>(bar_y_f + 0.5f);
		}

		// バーを描画（塗りつぶしあり）
		DrawBox(bar_x_, bar_y_, bar_x_ + bar_width_, bar_y_ + bar_height_, COLOR_WHITE, TRUE);
	}

	//-----------------------------------------------------------
	//! @brief コンテンツ全体の高さ設定
	//-----------------------------------------------------------
	void ScrollControlComponent::SetContentHeight(float height)
	{
		max_scroll_ = height;

		UpdateScrollRange();
	}

	//-----------------------------------------------------------
	//! @brief 表示領域の高さ設定
	//-----------------------------------------------------------
	void ScrollControlComponent::SetViewHeight(float height)
	{
		view_height_ = height;

		UpdateScrollRange();
	}

	//-----------------------------------------------------------
	//! @brief ズーム設定
	//-----------------------------------------------------------
	void ScrollControlComponent::SetZoom(float zoom)
	{
		zoom_ = zoom;

		UpdateScrollRange();
	}

	//-----------------------------------------------------------
	//! @brief スクロール速度設定
	//-----------------------------------------------------------
	void ScrollControlComponent::SetScrollSpeed(float speed)
	{
		scroll_speed_ = speed;
	}

	//-----------------------------------------------------------
	//! @brief バーの位置と幅設定
	//-----------------------------------------------------------
	void ScrollControlComponent::SetBarPosition(int x, int width)
	{
		bar_x_     = x;
		bar_width_ = width;
	}

	//-----------------------------------------------------------
	//! @brief スクロール位置取得
	//-----------------------------------------------------------
	float ScrollControlComponent::GetScroll() const
	{
		return scroll_;
	}

	//-----------------------------------------------------------
	//! @brief スクロール範囲とバーサイズを更新
	//-----------------------------------------------------------
	void ScrollControlComponent::UpdateScrollRange()
	{
		// ズーム後のコンテンツピクセル数を計算
		float content_pixel = max_scroll_ * zoom_;

		// 最大スクロール範囲を計算（コンテンツが表示領域より大きい場合のみ）
		max_scroll_range_ = (content_pixel > view_height_) ? (content_pixel - view_height_) : 0.0f;

		// バー高さ計算（拡大時はサムを小さく、縮小時はサムを大きくする）
		if (content_pixel > 0.0f)
		{
			float bar_height_f = (view_height_ * view_height_) / content_pixel;

			// 最小高さ制限
			if (bar_height_f < static_cast<float>(SCROLL_BAR_MIN_HEIGHT))
			{
				bar_height_f = static_cast<float>(SCROLL_BAR_MIN_HEIGHT);
			}

			bar_height_ = static_cast<int>(bar_height_f + 0.5f);
		}
		else
		{
			bar_height_ = static_cast<int>(view_height_);
		}

		// 現在のスクロール位置が範囲外なら補正
		if (scroll_ > max_scroll_range_)
		{
			scroll_ = max_scroll_range_;
		}
	}
}
