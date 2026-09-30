//----------------------------------------------------------------------------
//! @file   LoadProgressButton.cpp
//! @brief  途中データ読み込みボタンの実装
//! @detail CSVファイルと音源を読み込み、途中から譜面を再開する機能を提供します。
//----------------------------------------------------------------------------
#include "LoadProgressButton.h"
#include "UIConstants.h"
#include "GameConstants.h"
#include "Font.h"
#include "Colors.h"
#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace UI
{
	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void LoadProgressButton::OnInit()
	{
		Init();
		SetOff();
		load_success_ = false;
		load_message_timer_ = 0;
		loaded_notes_.clear();
		music_file_path_.clear();
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void LoadProgressButton::OnUpdate()
	{
		Update();

		// ボタンがONになった瞬間
		if (IsOn())
		{
			//--------------------------------------------------------
			// CSVファイル選択ダイアログを表示
			//--------------------------------------------------------
			std::string csvPath;

			if (ShowFileDialog(csvPath, "CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0"))
			{
				//--------------------------------------------------------
				// CSVからノーツデータを読み込み
				//--------------------------------------------------------
				if (LoadNotesFromCSV(csvPath.c_str()))
				{
					//--------------------------------------------------------
					// 音源ファイルの読み込み
					//--------------------------------------------------------
					std::string musicPath;

					if (ShowFileDialog(musicPath, "Audio Files (*.mp3;*.wav;*.ogg)\0*.mp3;*.wav;*.ogg\0All Files (*.*)\0*.*\0"))
					{
						music_file_path_ = musicPath;
						load_success_ = true;
						load_message_timer_ = SAVE_MESSAGE_DISPLAY_TIME;
					}
				}
			}

			SetOff();
		}

		// タイマー更新
		if (load_message_timer_ > 0)
		{
			load_message_timer_--;
		}

		if (load_message_timer_ == 0)
		{
			load_success_ = false;
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void LoadProgressButton::OnDraw()
	{
		Draw();

		// 読み込み成功メッセージ表示
		if (load_success_ && load_message_timer_ > 0)
		{
			const char* message = "読み込み完了";
			int x = 1000;
			int y = 200;

			DrawStringToHandle(x, y, message, COLOR_WHITE, Font::GetFont(Font::FontIndex::FONT_SIZE_40_INDEX));
		}
	}

	//-----------------------------------------------------------
	//! @brief 終了処理
	//-----------------------------------------------------------
	void LoadProgressButton::OnEnd()
	{
		End();
		loaded_notes_.clear();
	}

	//-----------------------------------------------------------
	//! @brief CSVファイルからノーツデータ読み込み
	//-----------------------------------------------------------
	bool LoadProgressButton::LoadNotesFromCSV(const char* filepath)
	{
		loaded_notes_.clear();

		std::ifstream file(filepath);

		if (!file.is_open())
		{
			return false;
		}

		std::string line;

		while (std::getline(file, line))
		{
			// 空行をスキップ
			if (line.empty()) continue;

			std::stringstream ss(line);
			std::string token;
			std::vector<int> values;

			// カンマ区切りで分割
			while (std::getline(ss, token, ','))
			{
				try
				{
					values.push_back(std::stoi(token));
				}
				catch (...)
				{
					continue;
				}
			}

			// 必要な要素数をチェック（time_ms, line_index, note_type）
			if (values.size() >= 3)
			{
				int timeMs = values[0];
				int lineIndex = values[1];
				int noteType = values[2];

				// レーン番号からX座標（レーン中央）を計算
				int laneWidth = static_cast<int>(DRAW_LINE_W);
				int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
				float posX = static_cast<float>(leftLaneStart + (lineIndex * laneWidth) + laneWidth / 2);

				// Y座標は時間から計算（ms -> 秒 -> ピクセル）。ZOOM_DEFAULT を基準とする
				float posY = (static_cast<float>(timeMs) / 1000.0f) * ZOOM_DEFAULT;

				// ノーツオブジェクトを作成して位置を設定（lineIndex などを正しく算出させるため SetPosition を呼ぶ）
				auto note = CreateNoteObject(noteType, posX, posY, timeMs);
				note->SetPosition(posX, posY);

				loaded_notes_.push_back(note);
			}
		}

		file.close();

		return !loaded_notes_.empty();
	}

	//-----------------------------------------------------------
	//! @brief ファイル選択ダイアログ表示
	//-----------------------------------------------------------
	bool LoadProgressButton::ShowFileDialog(std::string& filepath, const char* filter)
	{
		char filename[MAX_PATH] = {};

		OPENFILENAMEA ofn{};
		ofn.lStructSize = sizeof(OPENFILENAMEA);
		ofn.hwndOwner = GetMainWindowHandle();
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		ofn.lpstrTitle = "ファイルを選択";

		if (GetOpenFileNameA(&ofn))
		{
			filepath = filename;
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------
	//! @brief 読み込み済みデータを取得して内部状態をクリア
	//-----------------------------------------------------------
	bool LoadProgressButton::ConsumeLoadSuccess(std::vector<std::shared_ptr<NoteObject>>& outNotes, std::string& outMusicPath)
	{
		if (!load_success_) return false;

		outNotes = std::move(loaded_notes_);
		outMusicPath = std::move(music_file_path_);

		loaded_notes_.clear();
		music_file_path_.clear();
		load_success_ = false;
		load_message_timer_ = 0;

		return true;
	}
}
