//----------------------------------------------------------------------------
//! @file   DataSaveButton.cpp
//! @brief  データ保存ボタンクラスの実装
//! @detail ノーツ配置オブジェクトのデータをCSVに保存し、音源を所定の場所にコピーします。
//----------------------------------------------------------------------------
#include "DataSaveButton.h"
#include "UIConstants.h"
#include "GameConstants.h"
#include "GameScene.h"
#include "NotePlacerObject.h"
#include "MyLibraryFunctions.h"
#include <DxLib.h>
#include <algorithm>
#include <filesystem>
#include "Font.h"

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void DataSaveButton::OnInit()
	{
		Init();
		SetOff();
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void DataSaveButton::OnUpdate()
	{
		Update();

		// ボタンがONになった瞬間
		if (IsOn())
		{
			//--------------------------------------------------------
			// CSVに保存
			//--------------------------------------------------------
			const char* csvPath = "data/csv/data.csv";

			NotesToCSV(csvPath);

			//--------------------------------------------------------
			// 音源をコピー（決め打ち）
			//--------------------------------------------------------
			if (!music_file_path_.empty())
			{
				try
				{
					const std::string musicDst = "data/music/music.mp3";

					// コピー先フォルダが存在しなければ作成
					std::filesystem::path dstPath(musicDst);
					std::filesystem::create_directories(dstPath.parent_path());

					// コピーを実行（既存ファイルは上書き）
					std::filesystem::copy_file(music_file_path_, musicDst,
					                          std::filesystem::copy_options::overwrite_existing);
				}
				catch (const std::exception& e)
				{
					printf("音源コピー失敗: %s\n", e.what());
				}
			}

			// 保存成功フラグを設定
			saveSuccess_      = true;
			saveMessageTimer_ = SAVE_MESSAGE_DISPLAY_TIME;
		}

		// タイマー更新
		if (saveMessageTimer_ > 0)
		{
			saveMessageTimer_--;
		}

		if (saveMessageTimer_ == 0)
		{
			saveSuccess_ = false;
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void DataSaveButton::OnDraw()
	{
		Draw();

		if (saveSuccess_ && saveMessageTimer_ > 0)
		{
			const char* message = "保存完了";
			int x = 1000;
			int y = 150;

			DrawStringToHandle(x, y, message, COLOR_WHITE,
			                   Font::GetFont(Font::FontIndex::FONT_SIZE_40_INDEX));
		}
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void DataSaveButton::OnEnd()
	{
		End();
	}

	//-----------------------------------------------------------
	//! @brief NotePlacerObject設定
	//-----------------------------------------------------------
	void DataSaveButton::SetNotePlacerObject(const std::vector<std::shared_ptr<NoteObject>>& obj)
	{
		notes_ = obj;
	}

	//-----------------------------------------------------------
	//! @brief ノーツデータをCSVに保存
	//-----------------------------------------------------------
	void DataSaveButton::NotesToCSV(const char* filepath)
	{
		// 保存先フォルダが存在しなければ作成
		std::filesystem::path dstPath(filepath);
		std::filesystem::create_directories(dstPath.parent_path());

		FILE* f = nullptr;

		if (fopen_s(&f, filepath, "w")) return;

		for (const auto& note_ptr : notes_)
		{
			if (!note_ptr) continue;

			NoteObject* note = note_ptr.get();

			fprintf(f, "%d,%d,%d\n",
			        note->GetFrameTime(),
			        note->GetLineIndex(),
			        note->GetType());
		}

		fclose(f);
	}
}