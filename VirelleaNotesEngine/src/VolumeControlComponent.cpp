//----------------------------------------------------------------------------
//! @file   VolumeControlComponent.cpp
//! @brief  音量調整コンポーネントの実装
//! @detail 音量の調整、取得、設定機能を提供します。
//----------------------------------------------------------------------------
#include "VolumeControlComponent.h"
#include "AudioConstants.h"
#include "Font.h"
#include "Colors.h"
#include "UIConstants.h"
#include <algorithm>
#include <DxLib.h>

namespace Audio
{
	//-----------------------------------------------------------
	//! @brief コンストラクタ
	//-----------------------------------------------------------
	VolumeControlComponent::VolumeControlComponent()
		: volume_(VOLUME_DEFAULT)      // 音量初期化
		, hold_frame_count_(0)         // 押下フレーム数初期化
	{
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void VolumeControlComponent::Update()
	{
		// 上下キーの状態取得
		bool up   = CheckHitKey(KEY_INPUT_UP);
		bool down = CheckHitKey(KEY_INPUT_DOWN);

		if (up || down)
		{
			hold_frame_count_++;

			if (hold_frame_count_ >= VOLUME_KEY_HOLD_THRESHOLD)
			{
				// 一定フレーム以上押下している場合、毎フレーム増減
				if (up)   volume_ = min(volume_ + VOLUME_STEP, VOLUME_MAX);
				if (down) volume_ = max(volume_ - VOLUME_STEP, VOLUME_MIN);
			}
			else if (hold_frame_count_ == 1)
			{
				// 最初の瞬間のみ1増減
				if (up)   volume_ = min(volume_ + 1, VOLUME_MAX);
				if (down) volume_ = max(volume_ - 1, VOLUME_MIN);
			}
		}
		else
		{
			// キーを離したらフレームカウントをリセット
			hold_frame_count_ = 0;
		}
	}

	//-----------------------------------------------------------
	//! @brief 現在音量取得
	//-----------------------------------------------------------
	int VolumeControlComponent::GetVolume() const
	{
		return volume_;
	}

	//-----------------------------------------------------------
	//! @brief 音量設定
	//-----------------------------------------------------------
	void VolumeControlComponent::SetVolume(int volume)
	{
		volume_ = max(VOLUME_MIN, min(volume, VOLUME_MAX));
	}
}
