//----------------------------------------------------------------------------
//! @file   NoteObject.cpp
//! @brief  ノーツオブジェクトの実装
//! @detail ノーツの描画、位置設定、タイプ管理などの機能を提供します。
//----------------------------------------------------------------------------
#include "NoteObject.h"
#include "GameConstants.h"
#include "UIConstants.h"
#include "MyLibraryFunctions.h"

namespace UI
{
	//-----------------------------------------------------------
	//! @brief コンストラクタ
	//-----------------------------------------------------------
	NoteObject::NoteObject(int note_type, float x, float y, int time_ms)
		: pos_x_(x)             // X座標初期化
		, pos_y_(y)             // Y座標初期化
		, note_type_(note_type) // ノーツタイプ初期化
		, time_frame_(time_ms)  // 時間初期化
	{
		// ノーツ画像の初期化
		for (int i = 0; i < NOTE_TYPE_MAX; ++i)
		{
			notes_images_[i].Init();
			notes_images_[i].SetPos({ 0.0f, 0.0f });
		}

		// ノーツ画像の設定（ImageObject内部で既存画像を差し替えるため重複読み込みは防止）
		notes_images_[NOTE_TYPE_TAP].SetPath(TAP_PATH);
		notes_images_[NOTE_TYPE_HOLD_START].SetPath(HOLD_START_PATH);
		notes_images_[NOTE_TYPE_HOLD_END].SetPath(HOLD_END_PATH);
		notes_images_[NOTE_TYPE_SYNC].SetPath(SYNC_PATH);

		// line index を初期位置から算出（UI_OFFSET_X を考慮）
		SetPosition(pos_x_, pos_y_);
	}

	//-----------------------------------------------------------
	//! @brief ノーツの位置設定
	//-----------------------------------------------------------
	void NoteObject::SetPosition(float x, float y)
	{
		pos_x_ = x;
		pos_y_ = y;

		// レーン番号を計算
		int laneWidth = static_cast<int>(DRAW_LINE_W);
		int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
		int computedIndex = (static_cast<int>(x) - leftLaneStart) / laneWidth;
		if (computedIndex < 0) computedIndex = 0;
		if (computedIndex > 4) computedIndex = 4;
		line_index_ = computedIndex;
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void NoteObject::OnDraw(float scroll, float zoom)
	{
		// 描画用キャッシュの更新
		draw_x_ = pos_x_ + UI_OFFSET_X;
		draw_zoom_ratio_ = (zoom > 0.0f ? (zoom / ZOOM_DEFAULT) : 1.0f);
		draw_y_ = (pos_y_ * draw_zoom_ratio_) - scroll + UI_OFFSET_Y;

		notes_images_[note_type_].SetPos({ draw_x_ - IMAGE_OFFSET_X, draw_y_ - IMAGE_OFFSET_Y });

		// ノーツ画像の更新
		notes_images_[note_type_].Draw();
	}

	//-----------------------------------------------------------
	//! @brief ノーツのX座標取得
	//-----------------------------------------------------------
	float NoteObject::GetPosX() const
	{
		return pos_x_;
	}

	//-----------------------------------------------------------
	//! @brief ノーツのY座標取得
	//-----------------------------------------------------------
	float NoteObject::GetPosY() const
	{
		return pos_y_;
	}

	//-----------------------------------------------------------
	//! @brief ノーツのタイプ取得
	//-----------------------------------------------------------
	int NoteObject::GetType() const
	{
		return note_type_;
	}

	//-----------------------------------------------------------
	//! @brief ノーツの生成ミリ秒取得
	//-----------------------------------------------------------
	int NoteObject::GetFrameTime() const
	{
		return time_frame_;
	}

	//-----------------------------------------------------------
	//! @brief ノーツのライン番号取得
	//-----------------------------------------------------------
	int NoteObject::GetLineIndex() const
	{
		return line_index_;
	}

	//-----------------------------------------------------------
	//! @brief 初期化メソッド
	//-----------------------------------------------------------
	void NoteObject::Initialize(int note_type, float x, float y, int time_frame)
	{
		pos_x_       = x;
		pos_y_       = y;
		note_type_   = note_type;
		time_frame_  = time_frame;
		line_index_  = 0;
	}

	//-----------------------------------------------------------
	//! @brief NoteObject生成（shared_ptrで生成）
	//-----------------------------------------------------------
	std::shared_ptr<NoteObject> CreateNoteObject(int note_type, float x, float y, int time_ms)
	{
		// vector に格納して管理するため shared_ptr を返却
		auto note = std::make_shared<NoteObject>(note_type, x, y, time_ms);
		return note;
	}

	//-----------------------------------------------------------
	//! @brief NoteObject破棄（shared_ptrのリセット）
	//-----------------------------------------------------------
	void DestroyNoteObject(std::shared_ptr<NoteObject> note)
	{
		// vector から erase された際に shared_ptr が破棄されるため、
		// 呼び出し側が明示的に参照を破棄したい場合に備えて reset を行う	
		note.reset();
	}

	//-----------------------------------------------------------
	//! @brief ノーツの生成時間を秒で設定
	//-----------------------------------------------------------
	void NoteObject::SetTimeFromSeconds(float seconds)
	{
		time_frame_ = MyLibrary::SecondsToFrames(seconds) * (1000 / FRAME_RATE);
	}

	//-----------------------------------------------------------
	//! @brief ノーツの生成時間設定
	//-----------------------------------------------------------
	void NoteObject::SetTimeMs(int timeMs)
	{
		time_frame_ = timeMs;
	}
}