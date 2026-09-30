//----------------------------------------------------------------------------
//! @file   ZoomComponent.cpp
//! @brief  ズーム制御コンポーネントの実装
//! @detail ズーム倍率の調整、取得、設定機能を提供します。
//----------------------------------------------------------------------------
#include "ZoomComponent.h"
#include "GameConstants.h"
#include "InputManager.h"
#include <DxLib.h>
#include <algorithm>

namespace System
{
	//-----------------------------------------------------------
	//! @brief コンストラクタ
	//-----------------------------------------------------------
	ZoomComponent::ZoomComponent()
		: zoom_(ZOOM_DEFAULT)      // ズーム倍率初期化
		, zoom_step_(ZOOM_STEP)    // ステップ初期化
		, zoom_min_(ZOOM_MIN)      // 最小倍率初期化
		, zoom_max_(ZOOM_MAX)      // 最大倍率初期化
	{
	}

	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void ZoomComponent::Init()
	{
		zoom_ = ZOOM_DEFAULT;
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void ZoomComponent::Update()
	{
		// Ctrlキーが押されていない場合は何もしない
		if (!CheckHitKey(KEY_INPUT_LCONTROL) && !CheckHitKey(KEY_INPUT_RCONTROL))
			return;

		// マウスホイール入力取得
		int wheel = InputManager::InputManager::GetMouseWheel();

		// ズーム増減
		if (wheel > 0)
			zoom_ += zoom_step_;
		else if (wheel < 0)
			zoom_ -= zoom_step_;

		// 範囲制限
		if (zoom_ < zoom_min_) zoom_ = zoom_min_;
		if (zoom_ > zoom_max_) zoom_ = zoom_max_;
	}

	//-----------------------------------------------------------
	//! @brief 現在のズーム倍率取得
	//-----------------------------------------------------------
	float ZoomComponent::GetZoom() const
	{
		return zoom_;
	}

	//-----------------------------------------------------------
	//! @brief ズーム倍率設定
	//-----------------------------------------------------------
	void ZoomComponent::SetZoom(float zoom)
	{
		// 範囲内に収める
		if (zoom < zoom_min_) zoom = zoom_min_;
		if (zoom > zoom_max_) zoom = zoom_max_;

		zoom_ = zoom;
	}
}
