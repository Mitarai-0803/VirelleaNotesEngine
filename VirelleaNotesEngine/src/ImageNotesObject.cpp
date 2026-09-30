//----------------------------------------------------------------------------
//! @file   ImageNotesObject.cpp
//! @brief  ノーツタイプ画像オブジェクトの実装
//! @detail 現在選択中のノーツタイプ画像の初期化・更新・描画処理を提供します。
//----------------------------------------------------------------------------
#include "ImageNotesObject.h"
#include "UIConstants.h"
#include "DxLib.h"

namespace UI
{
	//-----------------------------------------------------------
	//! @brief デストラクタ
	//-----------------------------------------------------------
	ImageNotesObject::~ImageNotesObject()
	{
		for (int i = 0; i < NOTE_TYPE_MAX; ++i)
		{
			notes_images_[i].~ImageObject();
		}
	}

	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void ImageNotesObject::OnInit()
	{
		// ノーツ画像の初期化
		for (int i = 0; i < NOTE_TYPE_MAX; ++i)
		{
			notes_images_[i].Init();
			notes_images_[i].SetPos({ 0.0f, 0.0f });
		}

		// ノーツ画像の設定
		notes_images_[NOTE_TYPE_TAP].SetPath(TAP_PATH);
		notes_images_[NOTE_TYPE_HOLD_START].SetPath(HOLD_START_PATH);
		notes_images_[NOTE_TYPE_HOLD_END].SetPath(HOLD_END_PATH);
		notes_images_[NOTE_TYPE_SYNC].SetPath(SYNC_PATH);
		notes_images_[NOTE_TYPE_DELETE].SetPath(DELETE_CURSOR_PATH);
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void ImageNotesObject::OnUpdate()
	{
		// マウス座標の取得
		int mx = 0;
		int my = 0;

		GetMousePoint(&mx, &my);

		// キャッシュ更新
		mouse_x_cache_ = mx;
		mouse_y_cache_ = my;

		// 現在のノーツタイプのみ位置更新
		notes_images_[current_note_type_].SetPos({ static_cast<float>(mouse_x_cache_), static_cast<float>(mouse_y_cache_) });
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void ImageNotesObject::OnDraw()
	{
		// ノーツ画像の更新
		notes_images_[current_note_type_].Draw();
	}

	//-----------------------------------------------------------
	//! @brief 現在のノーツタイプ設定
	//-----------------------------------------------------------
	void ImageNotesObject::SetCurrentNoteType(int note_type)
	{
		current_note_type_ = note_type;
	}
}
