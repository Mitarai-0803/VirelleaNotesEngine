//----------------------------------------------------------------------------
//! @file   VerticalLinesObject.cpp
//! @brief  縦線オブジェクトの実装
//! @detail 画面に縦線を描画するための機能を提供します。
//----------------------------------------------------------------------------
#include "VerticalLinesObject.h"
#include "GameConstants.h"
#include "Colors.h"

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void VerticalLinesObject::OnInit()
	{
		line_count_ = 4;
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void VerticalLinesObject::OnUpdate()
	{
		// 総幅と開始Xをキャッシュ
		total_width_cached_ = DRAW_LINE_W * line_count_;
		start_x_cached_     = HALF_SCREEN_W - total_width_cached_ * HALF + UI_OFFSET_X;

		// 各線のX位置キャッシュを用意（必要に応じて再確保）
		if (static_cast<int>(line_positions_.size()) != line_count_ + 1)
		{
			line_positions_.assign(line_count_ + 1, 0.0f);
		}

		for (int i = 0; i <= line_count_; ++i)
		{
			line_positions_[i] = start_x_cached_ + i * DRAW_LINE_W;
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void VerticalLinesObject::OnDraw()
	{
		for (int i = 0; i <= line_count_; ++i)
		{
			DrawLineAA(line_positions_[i], UI_OFFSET_Y,
			           line_positions_[i], SCREEN_H,
			           COLOR_WHITE, SCROLL_BAR_LINE_WIDTH);
		}
	}

	//-----------------------------------------------------------
	//! @brief 縦線本数設定
	//-----------------------------------------------------------
	void VerticalLinesObject::SetLineCount(int line_count)
	{
		line_count_ = line_count;
	}
}
