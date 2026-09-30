//----------------------------------------------------------------------------
//! @file   PlayTimeIndicatorObject.cpp
//! @brief  再生時間表示オブジェクトの実装
//! @detail 画面上に現在の再生位置を示す線と文字列を描画する機能を提供します。
//----------------------------------------------------------------------------
#include "PlayTimeIndicatorObject.h"
#include "Font.h"
#include "Colors.h"
#include "MyLibraryFunctions.h"

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::OnInit()
	{
		// デフォルトのフォント設定
		font_handle_ = GetFont(Font::FONT_SIZE_30_INDEX);
		color_       = COLOR_ORANGE;

		// デフォルトの描画幅を画面サイズから計算
		height_ = static_cast<int>(SCREEN_H - UI_OFFSET_Y - 100);

		// デフォルト位置
		pos_.x = static_cast<int>(GRID_X_INTERVAL_LONG);
		pos_.y = UI_OFFSET_Y;

		// ラベル幅をキャッシュ
		playing_label_width_ = GetDrawStringWidthToHandle("再生中", -1, font_handle_);
		stopped_label_width_ = GetDrawStringWidthToHandle("停止中", -1, font_handle_);

		// ラベルX座標の初期キャッシュ
		playing_label_x_ = static_cast<int>(SCREEN_W - pos_.x) + playing_label_width_;
		stopped_label_x_ = static_cast<int>(SCREEN_W - pos_.x) + stopped_label_width_;
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::OnUpdate()
	{
		// 再生時間からラインY座標を計算してキャッシュ
		float seconds = MyLibrary::ConvertMilliToSec(play_time_);

		if (max_play_time_ > 0)
		{
			float max_seconds = MyLibrary::ConvertMilliToSec(max_play_time_);

			if (seconds > max_seconds) seconds = max_seconds;

			draw_line_y_ = static_cast<int>(seconds * zoom_ - scroll_y_) + UI_OFFSET_Y;
		}
		else
		{
			draw_line_y_ = pos_.y;
		}

		// ラベルX位置が pos_ に依存するためキャッシュ更新
		playing_label_x_ = static_cast<int>(SCREEN_W - pos_.x) + playing_label_width_;
		stopped_label_x_ = static_cast<int>(SCREEN_W - pos_.x) + stopped_label_width_;
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::OnDraw()
	{
		// キャッシュ済みのラインYとラベル情報を使用して描画
		int line_y = draw_line_y_;

		const char* label = is_playing_ ? playing_label_text_ : stopped_label_text_;
		int label_x       = is_playing_ ? playing_label_x_     : stopped_label_x_;

		// ラベル描画
		DrawStringToHandle(label_x, line_y, label, color_, font_handle_);

		// ライン描画
		DrawLine(pos_.x, line_y, SCREEN_W - pos_.x, line_y, color_, static_cast<int>(SCROLL_BAR_LINE_WIDTH));
	}

	//-----------------------------------------------------------
	//! @brief 再生位置設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetPlayTime(int play_time)
	{
		play_time_ = play_time;
	}

	//-----------------------------------------------------------
	//! @brief 最大再生時間設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetMaxPlayTime(int max_play_time)
	{
		max_play_time_ = max_play_time;
	}

	//-----------------------------------------------------------
	//! @brief 描画位置設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetPosition(int x, int y)
	{
		pos_.x = x;
		pos_.y = y;
	}

	//-----------------------------------------------------------
	//! @brief ラインの描画範囲高さ設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetHeight(int height)
	{
		height_ = height;
	}

	//-----------------------------------------------------------
	//! @brief 文字描画に使うフォントハンドル設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetFontHandle(int handle)
	{
		font_handle_ = handle;
	}

	//-----------------------------------------------------------
	//! @brief 再生状態設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetIsPlaying(bool is_playing)
	{
		is_playing_ = is_playing;
	}

	//-----------------------------------------------------------
	//! @brief ズーム値設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetZoom(float zoom)
	{
		zoom_ = zoom;
	}

	//-----------------------------------------------------------
	//! @brief スクロールY値設定
	//-----------------------------------------------------------
	void PlayTimeIndicatorObject::SetScrollY(float scroll_y)
	{
		scroll_y_ = scroll_y;
	}
}
