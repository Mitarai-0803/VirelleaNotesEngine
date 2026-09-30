//----------------------------------------------------------------------------
//! @file   PlayBarObject.cpp
//! @brief  再生バーオブジェクトの実装
//! @detail 縦の再生バーを描画し、再生位置の変更をサポートします。
//----------------------------------------------------------------------------
#include "PlayBarObject.h"
#include "Font.h"
#include "Colors.h"
#include <cmath>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void PlayBarObject::OnInit()
	{
		max_play_time_ms_     = 0;
		current_play_time_ms_ = 0;
		is_dragging_          = false;
		seek_requested_       = false;
		requested_seek_time_  = 0;
		progress_             = 0.0f;
		draw_progress_y_      = PLAY_BAR_Y;
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void PlayBarObject::OnUpdate()
	{
		// マウス入力処理
		int mouse_x, mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);

		// バーの範囲内かチェック（効率化のため先にXチェック）
		bool is_in_bar = (mouse_x >= PLAY_BAR_X && mouse_x <= PLAY_BAR_X + PLAY_BAR_WIDTH &&
			mouse_y >= PLAY_BAR_Y && mouse_y <= PLAY_BAR_Y + PLAY_BAR_HEIGHT);

		if (is_in_bar)
		{
			if (GetMouseInput() & MOUSE_INPUT_LEFT)
			{
				if (!is_dragging_)
				{
					is_dragging_ = true;
				}

				// ドラッグ中は再生時間を更新
				int new_time = CalculatePlayTimeFromMouseY(mouse_y);

				if (new_time != current_play_time_ms_)
				{
					current_play_time_ms_ = new_time;
					seek_requested_       = true;
					requested_seek_time_  = new_time;

					// 進捗割合を更新
					progress_       = static_cast<float>(new_time) / static_cast<float>(max_play_time_ms_);
					draw_progress_y_ = static_cast<int>(PLAY_BAR_Y + PLAY_BAR_HEIGHT * progress_);
				}
			}
			else
			{
				is_dragging_ = false;
			}
		}
		else
		{
			is_dragging_ = false;
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void PlayBarObject::OnDraw()
	{
		DrawPlayBar();
	}

	//-----------------------------------------------------------
	//! @brief 最大再生時間設定
	//-----------------------------------------------------------
	void PlayBarObject::SetMaxPlayTime(int max_play_time_ms)
	{
		max_play_time_ms_ = max_play_time_ms;
	}

	//-----------------------------------------------------------
	//! @brief 現在の再生時間設定
	//-----------------------------------------------------------
	void PlayBarObject::SetCurrentPlayTime(int current_play_time_ms)
	{
		current_play_time_ms_ = current_play_time_ms;

		// 進捗割合を計算してキャッシュ
		if (max_play_time_ms_ > 0)
		{
			progress_        = static_cast<float>(current_play_time_ms_) / static_cast<float>(max_play_time_ms_);
			draw_progress_y_ = static_cast<int>(PLAY_BAR_Y + PLAY_BAR_HEIGHT * progress_);
		}
		else
		{
			progress_        = 0.0f;
			draw_progress_y_ = PLAY_BAR_Y;
		}
	}

	//-----------------------------------------------------------
	//! @brief 現在の再生時間取得
	//-----------------------------------------------------------
	int PlayBarObject::GetCurrentPlayTime() const
	{
		return current_play_time_ms_;
	}

	//-----------------------------------------------------------
	//! @brief 再生位置変更イベント取得
	//-----------------------------------------------------------
	bool PlayBarObject::IsSeekRequested() const
	{
		return seek_requested_;
	}

	//-----------------------------------------------------------
	//! @brief 要求された再生位置取得
	//-----------------------------------------------------------
	int PlayBarObject::GetRequestedSeekTime() const
	{
		return requested_seek_time_;
	}

	//-----------------------------------------------------------
	//! @brief イベントリセット
	//-----------------------------------------------------------
	void PlayBarObject::ResetSeekRequest()
	{
		seek_requested_      = false;
		requested_seek_time_ = 0;
	}

	//-----------------------------------------------------------
	//! @brief 再生バー描画
	//-----------------------------------------------------------
	void PlayBarObject::DrawPlayBar()
	{
		if (max_play_time_ms_ <= 0) return;

		// バー背景描画
		DrawBox(PLAY_BAR_X, PLAY_BAR_Y,
			PLAY_BAR_X + PLAY_BAR_WIDTH, PLAY_BAR_Y + PLAY_BAR_HEIGHT,
			COLOR_GRAY, TRUE);

		// キャッシュされた進捗Yを使用
		int progress_y = draw_progress_y_;

		// 進捗バー描画（上→下）
		DrawBox(PLAY_BAR_X,
			PLAY_BAR_Y,
			PLAY_BAR_X + PLAY_BAR_WIDTH,
			progress_y,
			COLOR_ORANGE, TRUE);

		// 枠描画
		DrawBox(PLAY_BAR_X, PLAY_BAR_Y,
			PLAY_BAR_X + PLAY_BAR_WIDTH, PLAY_BAR_Y + PLAY_BAR_HEIGHT,
			COLOR_WHITE, FALSE);

		// 円の描画(最新の高さ)
		DrawCircle(PLAY_BAR_X + PLAY_BAR_WIDTH / 2,
			progress_y,
			PLAY_BAR_RUDIUS,
			COLOR_WHITE, TRUE);
	}

	//-----------------------------------------------------------
	//! @brief マウス位置から再生時間を計算
	//-----------------------------------------------------------
	int PlayBarObject::CalculatePlayTimeFromMouseY(int mouse_y)
	{
		// バー内の相対位置（0.0 = 上端, 1.0 = 下端）
		float ratio = static_cast<float>(mouse_y - PLAY_BAR_Y) /
			static_cast<float>(PLAY_BAR_HEIGHT);

		// 範囲クランプ
		if (ratio < 0.0f) ratio = 0.0f;
		if (ratio > 1.0f) ratio = 1.0f;

		// 上→下 に進むのでそのまま比率を使う
		return static_cast<int>(max_play_time_ms_ * ratio);
	}
}