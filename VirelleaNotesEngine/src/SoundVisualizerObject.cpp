//----------------------------------------------------------------------------
//! @file   SoundVisualizerObject.cpp
//! @brief  サウンドビジュアライザーの実装
//! @detail FFT解析結果を描画するビジュアライザー機能を提供します。
//----------------------------------------------------------------------------
#include "SoundVisualizerObject.h"
#include "LoadMusicComponent.h"
#include "GameConstants.h"
#include <algorithm>
#include <DxLib.h>
#include <cmath>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void SoundVisualizerObject::OnInit()
	{
		// サウンドビジュアライザーの座標設定
		sound_visualizer_pos_ = { 0, UI_OFFSET_Y + SOUND_VISUALOZER_OFFSET_Y };
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void SoundVisualizerObject::OnUpdate()
	{
		// 現在の再生位置からサンプルを使用して周波数分布を取得
		GetFFTVibrationSoftSound(soft_sound_handle_, -1, sample_pos_, 4096, param_list_, BUFFERLENGTH);

		// 描画用に各バーの半幅とY座標を事前計算してキャッシュ
		for (int i = 0; i < BUFFERLENGTH; ++i)
		{
			float param = static_cast<float>(std::pow(param_list_[i], 0.6)) * 8.0f;

			bar_half_widths_[i] = static_cast<int>(param * SOUNDLENGTH);
			upper_y_[i]         = ((BUFFERLENGTH - 1) - i) * 30 + sound_visualizer_pos_.y;
			lower_y_[i]         = (i * 30) + sound_visualizer_pos_.y;
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void SoundVisualizerObject::OnDraw()
	{
		// キャッシュされた描画パラメータを使用して描画
		for (int i = 0; i < BUFFERLENGTH; ++i)
		{
			int half = bar_half_widths_[i];

			// 上側のライン描画
			DrawBox(sound_visualizer_pos_.x - half, upper_y_[i],
			        sound_visualizer_pos_.x + half, upper_y_[i] + 20,
			        GetColor(255, 255, 255), TRUE);

			// 下側のライン描画
			DrawBox(sound_visualizer_pos_.x - half, lower_y_[i],
			        sound_visualizer_pos_.x + half, lower_y_[i] + 20,
			        GetColor(255, 255, 255), TRUE);
		}
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void SoundVisualizerObject::OnEnd()
	{
		// ソフトサウンドハンドル解放
		if (soft_sound_handle_ != 0)
		{
			DeleteSoftSound(soft_sound_handle_);
			soft_sound_handle_ = 0;
		}

		// サウンドハンドル解放
		if (sound_handle_ != 0)
		{
			DeleteSoundMem(sound_handle_);
			sound_handle_ = 0;
		}
	}

	//-----------------------------------------------------------
	//! @brief 解析音源ハンドル設定
	//-----------------------------------------------------------
	void SoundVisualizerObject::SetSoftHandle(int handle)
	{
		// 同一ハンドルなら何もしない
		if (handle == soft_sound_handle_)
		{
			return;
		}

		// 既存ハンドルをクリア
		if (soft_sound_handle_ != 0)
		{
			DeleteSoftSound(soft_sound_handle_);
			soft_sound_handle_ = 0;
		}

		if (sound_handle_ != 0)
		{
			DeleteSoundMem(sound_handle_);
			sound_handle_ = 0;
		}

		one_load_flag_ = false;

		// 新しいハンドルを設定（0 の場合はクリアのみ)
		soft_sound_handle_ = handle;

		if (soft_sound_handle_ != 0)
		{
			// ソフトサウンドから再生用ハンドルを作成
			sound_handle_  = LoadSoundMemFromSoftSound(soft_sound_handle_);
			one_load_flag_ = (sound_handle_ != 0);
		}
	}

	//-----------------------------------------------------------
	//! @brief 再生位置設定
	//-----------------------------------------------------------
	void SoundVisualizerObject::SetSampleTime(int sample_time)
	{
		sample_pos_ = sample_time;
	}

	//-----------------------------------------------------------
	//! @brief ビジュアライザー描画位置設定
	//-----------------------------------------------------------
	void SoundVisualizerObject::SetPosition(int x, int y)
	{
		sound_visualizer_pos_.x = x;
		sound_visualizer_pos_.y = y;
	}
}
