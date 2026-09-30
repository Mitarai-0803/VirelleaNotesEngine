//----------------------------------------------------------------------------
//! @file   DataSaveButton.h
//! @brief  ノーツデータ保存ボタンの宣言
//! @detail ノーツ配置オブジェクトのデータをCSVに保存するUIボタンを定義します。
//----------------------------------------------------------------------------
#pragma once
#include "ImageButton.h"
#include "NoteObject.h"
#include "NotePlacerObject.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace UI
{
	//============================================================================
	//! @class  DataSaveButton
	//! ノーツデータをCSVに保存するためのクラスです。
	//============================================================================
	class DataSaveButton : public ImageButton
	{
	public:
		//-----------------------------------------------------------
		//! @brief 初期化処理
		//-----------------------------------------------------------
		void OnInit() override;

		//-----------------------------------------------------------
		//! @brief 更新処理
		//-----------------------------------------------------------
		void OnUpdate() override;

		//-----------------------------------------------------------
		//! @brief 描画処理
		//-----------------------------------------------------------
		void OnDraw() override;

		//-----------------------------------------------------------
		//! @brief 終了処理
		//-----------------------------------------------------------
		void OnEnd() override;

		//-----------------------------------------------------------
		//! @brief NotePlacerObject設定
		//! @param [in] obj ノーツ配置オブジェクト
		//-----------------------------------------------------------
		void SetNotePlacerObject(const std::vector<std::shared_ptr<NoteObject>>& obj);

		//-----------------------------------------------------------
		//! @brief 音源ファイルパス設定
		//! @param [in] path 読み込んだ音源のフルパス
		//-----------------------------------------------------------
		void SetMusicFilePath(const std::string& path) { music_file_path_ = path; }

		//-----------------------------------------------------------
		//! @brief ノーツデータをCSVに保存
		//! @param [in] filepath 保存先ファイルパス
		//-----------------------------------------------------------
		void NotesToCSV(const char* filepath);

	private:
		std::vector<std::shared_ptr<NoteObject>> notes_;    //!< NotePlacerの保持するノーツ
		std::string music_file_path_ {};                    //!< 読み込んだ音源ファイルのフルパス
		bool saveSuccess_ = false;                          //!< 保存成功フラグ
		int saveMessageTimer_ = 0;                          //!< 保存完了メッセージのタイマー
		NotePlacerObject* notePlacerObject_ = nullptr;      //!< NotePlacerObjectへの参照
	};
}