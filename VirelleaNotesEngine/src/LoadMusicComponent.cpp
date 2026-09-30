//----------------------------------------------------------------------------
//! @file   LoadMusicComponent.cpp
//! @brief  音源読み込みコンポーネントの実装
//! @detail 音源ファイルの読み込み、再生管理、エラー処理を提供します。
//----------------------------------------------------------------------------
#include "LoadMusicComponent.h"
#include "MyLibraryFunctions.h"
#include "AudioConstants.h"
#include "GameConstants.h"
#include "UIConstants.h"
#include "Colors.h"
#include "Font.h"
#include <vector>
#include <string>
#include <DxLib.h>

namespace Audio
{
	//-----------------------------------------------------------
	//! @brief デストラクタ
	//-----------------------------------------------------------
	LoadMusicComponent::~LoadMusicComponent()
	{
		// ソフトサウンドハンドルが存在すれば解放
		if (soft_handle_ != 0)
		{
			DeleteSoftSound(soft_handle_);
			soft_handle_ = 0;
		}

		// 通常のサウンドハンドル解放
		if (sound_handle_ != 0)
		{
			DeleteSoundMem(sound_handle_);
			sound_handle_ = 0;
		}
	}

	//-----------------------------------------------------------
	//! @brief 初期化処理
	//-----------------------------------------------------------
	void LoadMusicComponent::Init()
	{
		sound_handle_ = 0;
		max_play_time_ = 0;
		reload_recast_count_ = 0;
		load_recast_frame_ = 0;

		file_path_.clear();
		file_path_buffer_.resize(MAX_PATH);

		normal_text_width_ = GetDrawStringWidthToHandle(NORMAL_LOAD_MSG, -1, GetFont(Font::FONT_SIZE_30_INDEX));
		error_text_width1_ = GetDrawStringWidthToHandle(ERROR_LOAD_MSG, -1, GetFont(Font::FONT_SIZE_30_INDEX));

		one_load_flag_ = false;
	}

	//-----------------------------------------------------------
	//! @brief 更新処理
	//-----------------------------------------------------------
	void LoadMusicComponent::Update()
	{
		// 既に読み込み済みならドラッグチェックをスキップ
		if (one_load_flag_)
		{
			return;
		}

		// ファイルが1つドラッグされているか
		if (GetDragFileNum() == 1)
		{
			file_path_buffer_.resize(MAX_PATH);
			GetDragFilePath(file_path_buffer_.data());
			file_path_buffer_.resize(strlen(file_path_buffer_.data()));

			std::string ext = MyLibrary::GetFileExtension(file_path_buffer_.data());

			// 対応拡張子のみ処理
			if (ext == EXT_MP3 || ext == EXT_WAV || ext == EXT_OGG)
			{
				file_path_ = file_path_buffer_.data();
				soft_handle_ = LoadSoftSound(file_path_buffer_.data());
				sound_handle_ = LoadSoundMemFromSoftSound(soft_handle_);
				max_play_time_ = GetSoundTotalTime(sound_handle_);
				one_load_flag_ = true;
			}
			else
			{
				// 非対応ファイルはエラー扱い。既存ソフトハンドルがあれば解放
				if (soft_handle_ != 0)
				{
					DeleteSoftSound(soft_handle_);
					soft_handle_ = 0;
				}

				sound_handle_ = SOUND_HANDLE_ERROR;
				reload_recast_count_ = RELOAD_WAIT_FRAMES;
			}

			SetDragFileValidFlag(FALSE);
		}

		// エラー時の再読み込み待機処理（一度読み込むまで有効）
		if (!one_load_flag_ && sound_handle_ == SOUND_HANDLE_ERROR)
		{
			if (reload_recast_count_ > 0)
			{
				reload_recast_count_--;
			}
			else
			{
				SetDragFileValidFlag(TRUE);
				sound_handle_ = 0;
			}
		}
	}

	//-----------------------------------------------------------
	//! @brief 描画処理
	//-----------------------------------------------------------
	void LoadMusicComponent::Draw()
	{
		// 読み込み前
		if (sound_handle_ == 0)
		{
			//	黒い背景を描画
			DrawBox(HALF_SCREEN_W - normal_text_width_ * 0.5f,
				HALF_SCREEN_H+1,
				HALF_SCREEN_W - normal_text_width_ * 0.5f + normal_text_width_,
				HALF_SCREEN_H + 31, COLOR_BLACK, TRUE);

			// 通常メッセージ描画
			DrawCenterString(NORMAL_LOAD_MSG, HALF_SCREEN_H, COLOR_WHITE, normal_text_width_);
		}
		// 読み込み失敗
		else if (sound_handle_ == SOUND_HANDLE_ERROR)
		{
			DrawCenterString(ERROR_LOAD_MSG, HALF_SCREEN_H, COLOR_WHITE, error_text_width1_);

			std::string reload_msg = std::to_string(reload_recast_count_ / 60) + RELOAD_MSG_SUFFIX;
			int reload_width = GetDrawStringWidthToHandle(reload_msg.c_str(), -1, GetFont(Font::FONT_SIZE_30_INDEX));

			DrawCenterString(reload_msg, HALF_SCREEN_H + 50, COLOR_WHITE, reload_width);
		}
	}

	//-----------------------------------------------------------
	//! @brief 画面中央に文字列描画
	//-----------------------------------------------------------
	void LoadMusicComponent::DrawCenterString(const std::string& text, int y, unsigned int color, int width)
	{
		int x = static_cast<int>(HALF_SCREEN_W - width * 0.5f);

		DrawStringToHandle(x, y, text.c_str(), color, GetFont(Font::FONT_SIZE_30_INDEX));
	}

	//-----------------------------------------------------------
	//! @brief 音源ハンドル取得
	//-----------------------------------------------------------
	int LoadMusicComponent::GetHandle() const
	{
		return sound_handle_;
	}

	//-----------------------------------------------------------
	//! @brief ソフト音源ハンドル取得
	//-----------------------------------------------------------
	int LoadMusicComponent::GetSoftHandle() const
	{
		return soft_handle_;
	}

	//-----------------------------------------------------------
	//! @brief 最大再生時間取得(ms)
	//-----------------------------------------------------------
	int LoadMusicComponent::GetMaxPlayTime() const
	{
		return max_play_time_;
	}

	//-----------------------------------------------------------
	//! @brief 指定ファイルから音源を読み込み
	//-----------------------------------------------------------
	bool LoadMusicComponent::LoadFromFile(const std::string& path)
	{
		if (path.empty()) return false;

		// 既存ハンドルがあれば解放
		if (soft_handle_ != 0)
		{
			DeleteSoftSound(soft_handle_);
			soft_handle_ = 0;
		}
		if (sound_handle_ != 0)
		{
			DeleteSoundMem(sound_handle_);
			sound_handle_ = 0;
		}

		soft_handle_ = LoadSoftSound(path.c_str());
		if (soft_handle_ == 0) return false;

		sound_handle_ = LoadSoundMemFromSoftSound(soft_handle_);
		if (sound_handle_ == 0)
		{
			DeleteSoftSound(soft_handle_);
			soft_handle_ = 0;
			return false;
		}

		max_play_time_ = GetSoundTotalTime(sound_handle_);
		file_path_ = path;
		one_load_flag_ = true;
		return true;
	}
}
