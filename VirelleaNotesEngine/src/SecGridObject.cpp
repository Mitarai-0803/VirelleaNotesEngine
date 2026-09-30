//----------------------------------------------------------------------------
//! @file   SecGridObject.cpp
//! @brief  秒グリッド描画オブジェクトの実装
//! @detail 秒単位のライン描画とスクロールに追従した描画処理を提供します。
//----------------------------------------------------------------------------
#include "SecGridObject.h"
#include "Font.h"
#include "Colors.h"
#include <cmath>
#include <algorithm>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void SecGridObject::OnInit()
	{
		max_play_time_ms_ = 0;
		zoom_             = ZOOM_DEFAULT;
		scroll_y_         = 0.0f;
		max_scroll_y_     = 0.0f;

		// キャッシュ初期化
		sec_count_      = 0;
		max_line_pos_y_ = 0;
		y_offset_       = 0.0f;
		start_i_        = 0;
		end_i_          = 0;

		// フォントハンドルをキャッシュ
		font_handle_ = GetFont(Font::FONT_SIZE_20_INDEX);
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void SecGridObject::OnUpdate()
	{
		// 総秒数と最大ライン位置を max_play_time_ms_ に依存するため更新
		sec_count_ = static_cast<int>(MyLibrary::ConvertMilliToSec(max_play_time_ms_));

		// 最大スクロールY座標を計算（キャッシュ）
		max_line_pos_y_ = static_cast<int>(static_cast<float>(sec_count_) * zoom_) - static_cast<int>(scroll_y_) + UI_OFFSET_Y;
		max_scroll_y_   = static_cast<float>(max_line_pos_y_ - UI_OFFSET_Y) + scroll_y_;

		// 画面に見えている秒の範囲を計算してキャッシュ
		y_offset_ = scroll_y_ - static_cast<float>(UI_OFFSET_Y);

		start_i_ = static_cast<int>(std::ceil(y_offset_ / zoom_));
		end_i_   = static_cast<int>(std::floor((y_offset_ + static_cast<float>(SCREEN_H)) / zoom_));

		// 範囲をクリップ
		start_i_ = (std::max)(0, start_i_);
		end_i_   = (std::min)(sec_count_, end_i_);

		// 描画用の初期状態を独立変数に設定し DrawSecGrid で随時更新する
		draw_start_       = start_i_;
		draw_end_         = end_i_;
		draw_y_           = static_cast<float>(draw_start_) * zoom_ - scroll_y_ + static_cast<float>(UI_OFFSET_Y);
		draw_mod_counter_ = draw_start_ % DRAW_GRID_INTERVAL;

		if (draw_mod_counter_ < 0) draw_mod_counter_ += DRAW_GRID_INTERVAL;

		draw_minutes_ = draw_start_ / SECONDS_PER_MIN;
		draw_seconds_ = draw_start_ % SECONDS_PER_MIN;

		// 描画で使う X 関係の初期値はここで決定しておく
		draw_start_x_ = GRID_X_INTERVAL_LONG;
		draw_end_x_   = SCREEN_W - GRID_X_INTERVAL_LONG;
		draw_color_   = COLOR_WHITE;
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void SecGridObject::OnDraw()
	{
		DrawSecGrid();
	}

	//-----------------------------------------------------------
	//! @brief 最大再生時間設定
	//-----------------------------------------------------------
	void SecGridObject::SetMaxPlayTime(int max_play_time_ms)
	{
		max_play_time_ms_ = max_play_time_ms;
	}

	//-----------------------------------------------------------
	//! @brief ズーム値設定
	//-----------------------------------------------------------
	void SecGridObject::SetZoom(float zoom)
	{
		zoom_ = zoom;
	}

	//-----------------------------------------------------------
	//! @brief スクロールY値設定
	//-----------------------------------------------------------
	void SecGridObject::SetScrollY(float scroll_y)
	{
		scroll_y_ = scroll_y;
	}

	//-----------------------------------------------------------
	//! @brief 最大スクロールY値取得
	//-----------------------------------------------------------
	float SecGridObject::GetMaxScrollY() const
	{
		return max_scroll_y_;
	}

	//-----------------------------------------------------------
	//! @brief 秒線の最後Y座標取得
	//-----------------------------------------------------------
	int SecGridObject::GetLinePosY() const
	{
		return line_pos_y_;
	}

	//-----------------------------------------------------------
	//! @brief 秒グリッド描画
	//-----------------------------------------------------------
	void SecGridObject::DrawSecGrid()
	{
		// キャッシュされた範囲内のみ描画
		if (draw_start_ > draw_end_) return;

		for (int i = draw_start_; i <= draw_end_; ++i)
		{
			line_pos_y_ = static_cast<int>(draw_y_);

			// X位置と色を独立変数で管理して更新
			draw_start_x_ = (draw_mod_counter_ == 0) ? GRID_X_INTERVAL_SHORT : GRID_X_INTERVAL_LONG;
			draw_end_x_   = (draw_mod_counter_ == 0) ? (SCREEN_W - GRID_X_INTERVAL_SHORT) : (SCREEN_W - GRID_X_INTERVAL_LONG);
			draw_color_   = (draw_mod_counter_ == 0) ? COLOR_GREEN : COLOR_WHITE;

			// 秒線の線の描画
			DrawLine(draw_start_x_ + UI_OFFSET_X, line_pos_y_,
			         draw_end_x_ + UI_OFFSET_X, line_pos_y_,
			         draw_color_);

			// ラベルY位置を独立変数に設定
			draw_label_y_ = (i == sec_count_) ? (line_pos_y_ - EXPANDED_GRID_LINE_OFFSET_Y)
			                                   : ((i == 0) ? line_pos_y_ : line_pos_y_ - GRID_LINE_OFFSET_Y);

			// 秒数テキストの描画
			DrawFormatStringToHandle(draw_start_x_ - GRID_LINE_OFFSET_X + UI_OFFSET_X, draw_label_y_,
			                         draw_color_, font_handle_, "%02d:%02d",
			                         draw_minutes_, draw_seconds_);

			// 次のループ用にメンバをインクリメント
			draw_y_ += zoom_;

			if (++draw_mod_counter_ >= DRAW_GRID_INTERVAL) draw_mod_counter_ = 0;

			if (++draw_seconds_ >= SECONDS_PER_MIN)
			{
				draw_seconds_ = 0;
				++draw_minutes_;
			}
		}
	}
}
