//----------------------------------------------------------------------------
//! @file   ScrollingBackgroundObject.cpp
//! @brief  スクロール背景オブジェクトの実装
//! @detail 背景画像を2枚使用して無限スクロールを実現します。
//!         画像が画面外に出たら上に戻すことでループを表現します。
//----------------------------------------------------------------------------
#include "ScrollingBackgroundObject.h"
#include "GameConstants.h"
#include "UIConstants.h"
#include <DxLib.h>

namespace UI
{
	//----------------------------------------------------------------------------
	//!	@brief 初期化処理
	//----------------------------------------------------------------------------
	void ScrollingBackgroundObject::OnInit()
	{
		//--------------------------------------------------------
		// 背景画像の読み込み
		//--------------------------------------------------------
		background_handle_ = LoadGraph(BACKGROUND_IMAGE_PATH);

		if (background_handle_ == -1)
		{
			// 画像読み込み失敗時のエラーログ
			printfDx("Error: Failed to load background image: %s\n", BACKGROUND_IMAGE_PATH);
			return;
		}

		//--------------------------------------------------------
		// 画像サイズの取得
		//--------------------------------------------------------
		int imageWidth{};
		GetGraphSize(background_handle_, &imageWidth, &image_height_);

		// スクロールオフセットの初期化
		scroll_offset_ = 0.0f;
		scroll_speed_ = BG_SCROLL_SPEED;
	}

	//----------------------------------------------------------------------------
	//! @brief 更新処理
	//----------------------------------------------------------------------------
	void ScrollingBackgroundObject::OnUpdate()
	{
		// 画像が読み込まれていない場合は処理しない
		if (background_handle_ == -1 || image_height_ <= 0)
		{
			return;
		}

		//--------------------------------------------------------
		// スクロールオフセットの更新
		//--------------------------------------------------------
		scroll_offset_ += scroll_speed_;

		// 画像1枚分スクロールしたら位置をリセット
		if (scroll_offset_ >= static_cast<float>(image_height_))
		{
			scroll_offset_ -= static_cast<float>(image_height_);
		}
	}

	//----------------------------------------------------------------------------
	//! @brief 描画処理
	//----------------------------------------------------------------------------
	void ScrollingBackgroundObject::OnDraw()
	{
		// 画像が読み込まれていない場合は処理しない
		if (background_handle_ == -1 || image_height_ <= 0)
		{
			return;
		}

		int offsetY = static_cast<int>(scroll_offset_);

		//--------------------------------------------------------
		// 1枚目の背景描画
		//--------------------------------------------------------
		// 画面上部から開始し、下方向にスクロール
		DrawGraphF(0, offsetY, background_handle_, TRUE);

		//--------------------------------------------------------
		// 2枚目の背景描画
		//--------------------------------------------------------
		// 1枚目の上に配置してループを実現
		DrawGraphF(0, offsetY - image_height_, background_handle_, TRUE);
	}

	//----------------------------------------------------------------------------
	//! @brief 終了処理
	//----------------------------------------------------------------------------
	void ScrollingBackgroundObject::OnEnd()
	{
		// 背景画像の解放
		DeleteGraph(background_handle_);
	}

	//----------------------------------------------------------------------------
	//! @brief スクロール速度を設定
	//----------------------------------------------------------------------------
	void ScrollingBackgroundObject::SetScrollSpeed(float speed)
	{
		scroll_speed_ = speed;
	}

	//----------------------------------------------------------------------------
	//! @brief 背景画像のハンドルを取得
	//----------------------------------------------------------------------------
	int ScrollingBackgroundObject::GetBackgroundHandle() const
	{
		return background_handle_;
	}

}
