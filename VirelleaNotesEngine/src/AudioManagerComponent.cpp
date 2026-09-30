//----------------------------------------------------------------------------
//! @file   AudioManagerComponent.cpp
//! @brief  音源管理コンポーネントの実装
//! @detail 音源の再生・停止、音量変更、現在位置の取得などの機能を提供します。
//----------------------------------------------------------------------------
#include "AudioManagerComponent.h"
#include <DxLib.h>

namespace Audio
{
	//-----------------------------------------------------------
	//! @brief コンストラクタ
	//-----------------------------------------------------------
	AudioManagerComponent::AudioManagerComponent()
		: load_handle_(0)      // 音源ハンドル初期化
		, stop_position_(0)    // 停止位置（ミリ秒）初期化
		, prev_volume_(0)      // 前フレームの音量初期化
		, current_volume_(0)   // 現在の音量初期化
	{
	}

	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void AudioManagerComponent::Init()
	{
		load_handle_     = 0;
		stop_position_   = 0;
		prev_volume_     = 0;
		current_volume_  = 0;
		sample_time_pos_ = 0;
		play_mode_       = false;
	}

	//-----------------------------------------------------------
	//! @brief 毎フレームの更新処理
	//-----------------------------------------------------------
	void AudioManagerComponent::Update()
	{
		ChangeVolumeSoundMem(current_volume_, load_handle_);
		sample_time_pos_ = GetCurrentPositionSoundMem(load_handle_);
		ms_time_pos_     = GetSoundCurrentTime(load_handle_);
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void AudioManagerComponent::End()
	{
		// ハンドルが有効かチェック
		if (load_handle_ != 0)
		{
			DeleteSoundMem(load_handle_);
			load_handle_ = 0;
		}
	}

	//-----------------------------------------------------------
	//! @brief 音源再生
	//-----------------------------------------------------------
	void AudioManagerComponent::HandlePlaySound()
	{
		// ハンドルが有効かつ再生停止中か確認
		if (load_handle_ != 0 && CheckSoundMem(load_handle_) == 0)
		{
			// 停止位置があればそこから再生
			if (stop_position_ != 0)
			{
				SetSoundCurrentTime(stop_position_, load_handle_);
			}

			PlaySoundMem(load_handle_, DX_PLAYTYPE_BACK, FALSE);
			play_mode_ = true;
		}
	}

	//-----------------------------------------------------------
	//! @brief 音源停止
	//-----------------------------------------------------------
	void AudioManagerComponent::HandleStopSound()
	{
		// ハンドルが有効かつ再生中か確認
		if (load_handle_ > 0 && CheckSoundMem(load_handle_) == 1)
		{
			StopSoundMem(load_handle_);
			stop_position_ = GetSoundCurrentTime(load_handle_);
			play_mode_     = false;
		}
	}

	//-----------------------------------------------------------
	//! @brief 音源ハンドル設定
	//-----------------------------------------------------------
	void AudioManagerComponent::SetHandle(int handle)
	{
		load_handle_ = handle;
	}

	//-----------------------------------------------------------
	//! @brief 音量設定
	//-----------------------------------------------------------
	void AudioManagerComponent::SetVolume(int volume)
	{
		current_volume_ = volume;
	}

	//-----------------------------------------------------------
	//! @brief 再生位置シーク
	//-----------------------------------------------------------
	void AudioManagerComponent::Seek(int msTime)
	{
		// ハンドルが有効
		if (load_handle_ != 0)
		{
			SetSoundCurrentTime(msTime, load_handle_);
			ms_time_pos_ = msTime;

			// 停止中にシークした場合、停止位置も更新しておく
			stop_position_ = msTime;
		}
	}

	//-----------------------------------------------------------
	//! @brief 停止位置取得
	//-----------------------------------------------------------
	int AudioManagerComponent::GetStopPosition() const
	{
		return stop_position_;
	}

	//-----------------------------------------------------------
	//! @brief 再生位置取得（サンプル単位）
	//-----------------------------------------------------------
	int AudioManagerComponent::GetSampleTime() const
	{
		return sample_time_pos_;
	}

	//-----------------------------------------------------------
	//! @brief 再生位置取得（ミリ秒単位）
	//-----------------------------------------------------------
	int AudioManagerComponent::GetMsTime() const
	{
		return ms_time_pos_;
	}

	//-----------------------------------------------------------
	//! @brief 再生状態取得
	//-----------------------------------------------------------
	bool AudioManagerComponent::GetPlayeMode() const
	{
		return play_mode_;
	}
}
