//----------------------------------------------------------------------------
//! @file   BpmControlComponent.cpp
//! @brief  BPM制御コンポーネントの実装
//! @detail BPMの調整、グリッド描画などの機能を提供します。
//----------------------------------------------------------------------------
#include "BpmControlComponent.h"
#include "Font.h"
#include "Colors.h"
#include "GameConstants.h"
#include "UIConstants.h"
#include "MyLibraryFunctions.h"
#include <algorithm>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief コンストラクタ
	//-----------------------------------------------------------
	BpmControlComponent::BpmControlComponent()
		: bpm_(BPM_DEFAULT)          // BPM初期化
		, hold_frame_count_(0)       // 押下フレーム数初期化
		, line_count_(4)             // ライン数初期化
		, max_play_time_ms_(0)       // 最大再生時間初期化
		, zoom_(1.0f)                // ズーム倍率初期化
		, scroll_y_(0)               // スクロール量Y初期化
	{
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void BpmControlComponent::Update()
	{
		// 左右キー状態を取得
		bool left  = CheckHitKey(KEY_INPUT_LEFT);
		bool right = CheckHitKey(KEY_INPUT_RIGHT);

		// キーが押されている場合
		if (left || right)
		{
			hold_frame_count_++;

			// 押下が閾値を超えた場合、毎フレームBPM増減
			if (hold_frame_count_ >= BPM_KEY_HOLD_THRESHOLD)
			{
				if (left)  bpm_ = min(bpm_ + BPM_STEP, BPM_MAX);
				if (right) bpm_ = max(bpm_ - BPM_STEP, BPM_MIN);
			}
			// 最初の瞬間のみ1増減
			else if (hold_frame_count_ == 1)
			{
				if (left)  bpm_ = min(bpm_ + 1, BPM_MAX);
				if (right) bpm_ = max(bpm_ - 1, BPM_MIN);
			}
		}
		else
		{
			hold_frame_count_ = 0;
		}

		SetBpm(bpm_);
	}

	//-----------------------------------------------------------
	//! @brief 現在BPM取得
	//-----------------------------------------------------------
	int BpmControlComponent::GetBpm() const
	{
		return bpm_;
	}

	//-----------------------------------------------------------
	//! @brief BPM設定
	//-----------------------------------------------------------
	void BpmControlComponent::SetBpm(int bpm)
	{
		bpm_ = std::clamp(bpm, BPM_MIN, BPM_MAX);
	}
}
