//----------------------------------------------------------------------------
//! @file   BpmGridObject.cpp
//! @brief  BPMグリッド描画オブジェクトの実装
//! @detail BPMに合わせたグリッド線の描画と表示範囲計算を行います。
//----------------------------------------------------------------------------
#include "BpmGridObject.h"
#include "Font.h"
#include "Colors.h"
#include "GameConstants.h"
#include "UIConstants.h"
#include "MyLibraryFunctions.h"
#include <cmath>
#include <algorithm>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void BpmGridObject::OnInit()
	{
		max_play_time_ms_ = 0;
		zoom_             = ZOOM_DEFAULT;
		scroll_y_         = 0.0f;
		bpm_              = BPM_DEFAULT;
		max_scroll_y_     = 0.0f;

		// キャッシュ初期化
		interval_sec_ = 0.0f;
		line_count_   = 0;
		y_offset_     = 0.0f;
		start_i_      = 0;
		end_i_        = 0;
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void BpmGridObject::OnUpdate()
	{
		// bpm が有効である場合拍間隔を計算してキャッシュ
		if (bpm_ > 0)
		{
			interval_sec_ = 60.0f / static_cast<float>(bpm_);
			line_count_   = static_cast<int>(MyLibrary::ConvertMilliToSec(max_play_time_ms_) / interval_sec_);

			// 最大スクロールY座標を計算
			max_scroll_y_ = static_cast<float>(line_count_) * interval_sec_ * zoom_;

			// 画面に見えているライン範囲を計算してキャッシュ
			y_offset_ = scroll_y_ - static_cast<float>(UI_OFFSET_Y);

			float visible_top    = y_offset_;
			float visible_bottom = y_offset_ + static_cast<float>(SCREEN_H);

			start_i_ = static_cast<int>(std::ceil(visible_top / (interval_sec_ * zoom_)));
			end_i_   = static_cast<int>(std::floor(visible_bottom / (interval_sec_ * zoom_)));

			// クリップ
			start_i_ = (std::max)(0, start_i_);
			end_i_   = (std::min)(line_count_, end_i_);

			// 描画用の初期状態を独立変数に設定
			draw_start_      = start_i_;
			draw_end_        = end_i_;
			draw_y_          = static_cast<float>(draw_start_) * interval_sec_ * zoom_ - scroll_y_ + static_cast<float>(UI_OFFSET_Y);
			draw_start_x_    = DRAW_BPM_GRID_X + UI_OFFSET_X;
			draw_end_x_      = SCREEN_W - DRAW_BPM_GRID_X + UI_OFFSET_X;
			draw_color_      = COLOR_CYAN;
			draw_line_width_ = static_cast<int>(SCROLL_BAR_LINE_WIDTH);
		}
		else
		{
			max_scroll_y_ = 0.0f;
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void BpmGridObject::OnDraw()
	{
		DrawBpmGrid();
	}

	//-----------------------------------------------------------
	//! @brief 最大再生時間設定
	//-----------------------------------------------------------
	void BpmGridObject::SetMaxPlayTime(int max_play_time_ms)
	{
		max_play_time_ms_ = max_play_time_ms;
	}

	//-----------------------------------------------------------
	//! @brief ズーム値設定
	//-----------------------------------------------------------
	void BpmGridObject::SetZoom(float zoom)
	{
		zoom_ = zoom;
	}

	//-----------------------------------------------------------
	//! @brief スクロールY値設定
	//-----------------------------------------------------------
	void BpmGridObject::SetScrollY(float scroll_y)
	{
		scroll_y_ = scroll_y;
	}

	//-----------------------------------------------------------
	//! @brief BPM設定
	//-----------------------------------------------------------
	void BpmGridObject::SetBpm(int bpm)
	{
		bpm_ = bpm;
	}

	//-----------------------------------------------------------
	//! @brief 最大スクロールY値取得
	//-----------------------------------------------------------
	float BpmGridObject::GetMaxScrollY() const
	{
		return max_scroll_y_;
	}

	//-----------------------------------------------------------
	//! @brief BPMグリッド描画
	//-----------------------------------------------------------
	void BpmGridObject::DrawBpmGrid()
	{
		if (bpm_ <= 0) return;

		// 随時更新で描画
		for (int i = draw_start_; i <= draw_end_; ++i)
		{
			int yPos = static_cast<int>(draw_y_);

			// ライン描画
			DrawLine(draw_start_x_, yPos, draw_end_x_, yPos, draw_color_, draw_line_width_);

			// ラベルY位置計算
			if (i == line_count_)
			{
				draw_label_y_ = yPos - EXPANDED_GRID_LINE_OFFSET_Y;
			}
			else
			{
				draw_label_y_ = (i == 0) ? yPos : yPos - GRID_LINE_OFFSET_Y;
			}

			// 次のライン用に随時更新
			draw_y_ += interval_sec_ * zoom_;
		}
	}
}
