#pragma once
#include "SceneBase.h"
#include "GameObject.h"
#include "ScrollControlComponent.h"
#include "VolumeControlComponent.h"
#include "BpmControlComponent.h"
#include "LoadMusicComponent.h"
#include "MyLibraryFunctions.h"
#include "GameConstants.h"
#include "ZoomComponent.h"
#include "GridComponent.h"
#include "AudioManagerComponent.h"
#include "ImageButton.h"
#include <memory>

namespace Scene
{
	//-----------------------------------------------------------
	//! @class GameScene
	//! @brief ゲーム本編を管理するシーンクラス
	//! @details　各種 UI コンポーネントや音楽関連コンポーネントを登録する
	//! Scene のライフサイクル（初期化、更新、描画、終了）を制御
	//-----------------------------------------------------------
	class GameScene : public SceneBase 
	{
	private:
		//-------------------------------------------------------
		//! @brief シーン初期化処理
		//! @details コンポーネントを `GameObject` に登録し、シーンに追加する。
		//-------------------------------------------------------
		void SceneInit() override;
			
		//-------------------------------------------------------
		//! @brief シーン更新処理
		//! @details 登録された `GameObject` およびコンポーネントを毎フレーム更新する。
		//-------------------------------------------------------
		void SceneUpdate() override;

		//-------------------------------------------------------
		//! @brief シーン描画処理
		//! @details 登録された `GameObject` およびコンポーネントを描画する。
		//-------------------------------------------------------
		void SceneDraw() override;

		//-------------------------------------------------------
		//! @brief シーン終了処理
		//! @details 登録された `GameObject` やコンポーネントの終了処理を呼び出す。
		//-------------------------------------------------------
		void SceneEnd() override;

	private:
		bool showUsage_ = false; //!< 操作説明表示フラグ
	};
}