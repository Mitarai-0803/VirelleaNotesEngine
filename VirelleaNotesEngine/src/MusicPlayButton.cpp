//----------------------------------------------------------------------------
//! @file   MusicPlayButton.cpp
//! @brief  音楽再生ボタンの実装
//! @detail 音楽の再生・停止を制御するボタンの機能を提供します。
//----------------------------------------------------------------------------
#include "MusicPlayButton.h"
#include "AudioManagerComponent.h"
#include "UIConstants.h"
#include "GameConstants.h"
#include <DxLib.h>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void MusicPlayButton::OnInit()
	{
		// ハンドルの初期化
		handle_ = 0;

		// ボタン画像と座標を設定
		Init();
		SetInputType(UI::ButtonInputType::TOGGLE);
		SetOff();
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void MusicPlayButton::OnUpdate()
	{
		auto audio_mnager = GetComponent<Audio::AudioManagerComponent>();

		// audio_mnagerがnullでない かつ ハンドルが0でない場合
		if (audio_mnager)
		{
			// ハンドルを取得して設定
			audio_mnager->SetHandle(handle_);

			// 音量を設定
			audio_mnager->SetVolume(volume_);

			// ONになった瞬間に再生
			if (IsOn() && !was_on_)
			{
				if (seek_position_ != 0)
				{
					audio_mnager->Seek(seek_position_);
					seek_position_ = 0;
				}

				audio_mnager->HandlePlaySound();
			}
			// OFFになった瞬間に停止
			else if (!IsOn() && was_on_)
			{
				audio_mnager->HandleStopSound();
			}
		}

		was_on_ = IsOn();

		Update();
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void MusicPlayButton::OnDraw()
	{
		Draw();
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void MusicPlayButton::OnEnd()
	{
		End();
	}

	//-----------------------------------------------------------
	//! @brief 音源ハンドル設定
	//-----------------------------------------------------------
	void MusicPlayButton::SetHandle(int music_handle)
	{
		handle_ = music_handle;
	}

	//-----------------------------------------------------------
	//! @brief 音源の音量設定
	//-----------------------------------------------------------
	void MusicPlayButton::SetVolume(int volume)
	{
		volume_ = volume;
	}

	//-----------------------------------------------------------
	//! @brief シーク位置設定
	//-----------------------------------------------------------
	void MusicPlayButton::SetSeekPosition(int seekPosition)
	{
		seek_position_ = seekPosition;
	}
}