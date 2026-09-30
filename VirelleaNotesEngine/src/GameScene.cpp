//----------------------------------------------------------------------------
//! @file   GameScene.cpp
//! @brief  ゲームシーンの実装
//! @detail シーンの初期化、更新、描画、終了処理を統括し、各UIオブジェクトやコンポーネント間の連携を管理します。
//----------------------------------------------------------------------------
#include "GameScene.h"
#include "MusicPlayButton.h"
#include "NoteObject.h"
#include "NotePlacerObject.h"
#include "DataSaveButton.h"
#include "LoadProgressButton.h"
#include "SoundVisualizerObject.h"
#include "SecGridObject.h"
#include "BpmGridObject.h"
#include "VerticalLinesObject.h"
#include "DrawBoxComponent.h"
#include "DrawFormatStringObject.h"
#include "Font.h"
#include "NoteTypeButton.h"
#include "ImageNotesObject.h"
#include "PlayTimeIndicatorObject.h"
#include "UsageButton.h"
#include "PlayBarObject.h"
#include <vector>
#include <string>
#include "ScrollingBackgroundObject.h"

namespace Scene
{
	//-----------------------------------------------------------
	//! @brief シーン初期化
	//-----------------------------------------------------------
	void GameScene::SceneInit()
	{
		// 秒グリッドオブジェクト
		AddObjectSetName<UI::SecGridObject>("SecGridObject");

		// BPMグリッドオブジェクト
		AddObjectSetName<UI::BpmGridObject>("BpmGridObject");

		// 縦グリッドオブジェクト
		AddObjectSetName<UI::VerticalLinesObject>("VerticalLinesObject");

		// ノーツ配置オブジェクト
		AddObjectSetName<UI::NotePlacerObject>("NotePlacer");

		// 再生中ラインオブジェクト
		AddObjectSetName<UI::PlayTimeIndicatorObject>("PlayTimeIndicator");

		// 再生バーオブジェクト
		AddObjectSetName<UI::PlayBarObject>("PlayBarObject");

		// 背景ボックスオブジェクト
		auto box_obj = std::make_shared<GameObject>();
		box_obj->SetName("BackgroundBox");
		box_obj->AddComponent<UI::DrawBoxComponent>();
		AddObject(box_obj);

		// UI関連オブジェクト
		auto ui_obj = std::make_shared<GameObject>();
		ui_obj->SetName("UIManagerObject");
		ui_obj->AddComponent<System::ScrollControlComponent>();
		ui_obj->AddComponent<System::ZoomComponent>();
		ui_obj->AddComponent<UI::BpmControlComponent>();
		AddObject(ui_obj);

		// 音楽関連オブジェクト
		auto music_obj = std::make_shared<GameObject>();
		music_obj->SetName("MusicObject");
		music_obj->AddComponent<Audio::LoadMusicComponent>();
		music_obj->AddComponent<Audio::VolumeControlComponent>();
		AddObject(music_obj);

		// BPM文字列オブジェクト
		AddObjectSetName<UI::DrawFormatStringObject>("BpmStringObject");

		// 音量文字列オブジェクト
		AddObjectSetName<UI::DrawFormatStringObject>("VolumeStringObject");

		// 再生時間文字列オブジェクト
		AddObjectSetName<UI::DrawFormatStringObject>("PlayTimeStringObject");

		// 再生ボタンオブジェクト
		AddObjectSetName<UI::MusicPlayButton>("MusicPlayButton");
		auto music_play_button = GetSceneObject<UI::MusicPlayButton>("MusicPlayButton");
		music_play_button->SetButtonImage(PLAY_BUTTON_PATH, STOP_BUTTON_PATH, { PLAY_BTN_X, BUTTON_Y });
		music_play_button->AddComponent<Audio::AudioManagerComponent>();

		// データ出力ボタンオブジェクト
		AddObjectSetName<UI::DataSaveButton>("DataSaveButton");

		// 途中読み込みボタンオブジェクト
		AddObjectSetName<UI::LoadProgressButton>("LoadProgressButton");

		// サウンドビジュアライザーオブジェクト
		AddObjectSetName<UI::SoundVisualizerObject>("SoundVisualizer");

		// ノーツタイプ画像オブジェクト
		AddObjectSetName<UI::ImageNotesObject>("ImageNotesObject");

		// ノーツタイプを変更するボタン (5種類) オブジェクト
		// 配列で名前・タイプ・画像パスをまとめて管理して一度に生成・設定する
		struct ButtonInfo { const char* name; UI::NOTE_TYPE type; const char* imgOn; const char* imgOff; };
		ButtonInfo buttons[] = {
			{ "NoteTypeTapButton"         , UI::NOTE_TYPE::NOTE_TYPE_TAP        , TAP_BUTTON_PATH, TAP_BUTTON_PATH },
			{ "NoteTypeHoldStartButton"   , UI::NOTE_TYPE::NOTE_TYPE_HOLD_START , HOLD_START_BUTTON_PATH, HOLD_START_BUTTON_PATH },
			{ "NoteTypeHoldEndButton"     , UI::NOTE_TYPE::NOTE_TYPE_HOLD_END   , HOLD_END_BUTTON_PATH, HOLD_END_BUTTON_PATH },
			{ "NoteTypeSyncButton"        , UI::NOTE_TYPE::NOTE_TYPE_SYNC       , SYNC_BUTTON_PATH, SYNC_BUTTON_PATH },
			{ "NoteTypeDeleteButton"      , UI::NOTE_TYPE::NOTE_TYPE_DELETE     , DELETE_BUTTON_PATH, DELETE_BUTTON_PATH }
		};

		for (const auto& button : buttons)
		{
			AddObjectSetName<UI::NoteTypeButton>(button.name);
		}

		//--------------------------------------------------------
		// 初期設定
		//--------------------------------------------------------

		// スクロール関連
		auto scroll = GetSceneObject<GameObject>("UIManagerObject")->GetComponent<System::ScrollControlComponent>();
		scroll->SetContentHeight(0);
		scroll->SetViewHeight(SCREEN_H);
		scroll->SetBarPosition(0, 0);
		scroll->SetScrollSpeed(20);

		// BPM文字列
		auto bpm_string = GetSceneObject<UI::DrawFormatStringObject>("BpmStringObject");
		bpm_string->SetColor(COLOR_WHITE);
		bpm_string->SetPos(BPM_STR_X, BPM_STR_Y);
		bpm_string->SetFormat(BPM_STR);
		bpm_string->SetFontHandle(GetFont(Font::FONT_SIZE_40_INDEX));

		// 音量文字列
		auto volume_string = GetSceneObject<UI::DrawFormatStringObject>("VolumeStringObject");
		volume_string->SetColor(COLOR_WHITE);
		volume_string->SetFormat(VOLUME_STR);
		volume_string->SetPos(VOLUME_STR_X, VOLUME_STR_Y);
		volume_string->SetFontHandle(GetFont(Font::FONT_SIZE_40_INDEX));

		// 再生時間文字列
		auto playTimeString = GetSceneObject<UI::DrawFormatStringObject>("PlayTimeStringObject");
		playTimeString->SetColor(COLOR_WHITE);
		playTimeString->SetFormat(TIME_STR);
		playTimeString->SetPos(PLAY_TIME_STR_X, PLAY_TIME_STR_Y);
		playTimeString->SetFontHandle(GetFont(Font::FONT_SIZE_40_INDEX));

		// データ出力ボタンオブジェクト
		auto data_save_button = GetSceneObject<UI::DataSaveButton>("DataSaveButton");
		data_save_button->SetInputType(UI::ButtonInputType::ONE_SHOT);
		data_save_button->SetButtonImage(SAVE_BUTTON_PATH, SAVE_BUTTON_PATH, { HALF_SCREEN_W - DATA_SAVE_BUTTON_X, BUTTON_Y });

		// 途中読み込みボタンオブジェクト
		auto load_progress_button = GetSceneObject<UI::LoadProgressButton>("LoadProgressButton");
		load_progress_button->SetInputType(UI::ButtonInputType::ONE_SHOT);
		load_progress_button->SetButtonImage(LOAD_PROGRESS_BUTTON_PATH, LOAD_PROGRESS_BUTTON_PATH, { LOAD_PROGRESS_BTN_X, LOAD_PROGRESS_BTN_Y });

		// 使い方ボタンオブジェクト
		AddObjectSetName<UI::UsageButton>("UsageButton");
		auto usageButton = GetSceneObject<UI::UsageButton>("UsageButton");

		// ノーツタイプ変更ボタン
		int index = 0;

		for (const auto& button : buttons)
		{
			auto buttonObj = GetSceneObject<UI::NoteTypeButton>(button.name);

			buttonObj->SetNoteType(button.type);
			buttonObj->SetInputType(UI::ButtonInputType::ONE_SHOT);
			buttonObj->SetButtonImage(button.imgOn, button.imgOff, { NOTES_TYPE_BTN_X + (NOTES_TYPE_BTN_W_SPACE * index), BUTTON_Y });

			index++;
		}

		// オブジェクトの名前の設定と追加
		AddObjectSetName<UI::ScrollingBackgroundObject>("ScrollingBackgroundObject");
	}

	//-----------------------------------------------------------
	//! @brief シーン更新
	//-----------------------------------------------------------
	void GameScene::SceneUpdate()
	{
		//--------------------------------------------------------
		// 各オブジェクトを取得
		//--------------------------------------------------------
		auto playBtn          = GetSceneObject<UI::MusicPlayButton>("MusicPlayButton");
		auto saveBtn          = GetSceneObject<UI::DataSaveButton>("DataSaveButton");
		auto loadProgressBtn  = GetSceneObject<UI::LoadProgressButton>("LoadProgressButton");
		auto visualizer       = GetSceneObject<UI::SoundVisualizerObject>("SoundVisualizer");
		auto secGridObj       = GetSceneObject<UI::SecGridObject>("SecGridObject");
		auto bpmGridObj       = GetSceneObject<UI::BpmGridObject>("BpmGridObject");
		auto verticalLinesObj = GetSceneObject<UI::VerticalLinesObject>("VerticalLinesObject");
		auto notePlacer       = GetSceneObject<UI::NotePlacerObject>("NotePlacer");
		auto playTimeLineObj  = GetSceneObject<UI::PlayTimeIndicatorObject>("PlayTimeIndicator");
		auto bpmString        = GetSceneObject<UI::DrawFormatStringObject>("BpmStringObject");
		auto volumeString     = GetSceneObject<UI::DrawFormatStringObject>("VolumeStringObject");
		auto playTimeString   = GetSceneObject<UI::DrawFormatStringObject>("PlayTimeStringObject");
		auto playBar          = GetSceneObject<UI::PlayBarObject>("PlayBarObject");
		auto background = GetSceneObject<UI::ScrollingBackgroundObject>("ScrollingBackgroundObject");

		//--------------------------------------------------------
		// 各オブジェクトについているコンポーネントを取得
		//--------------------------------------------------------
		auto scroll                 = GetSceneObject<GameObject>("UIManagerObject")->GetComponent<System::ScrollControlComponent>();
		auto zoom                   = GetSceneObject<GameObject>("UIManagerObject")->GetComponent<System::ZoomComponent>();
		auto bpm                    = GetSceneObject<GameObject>("UIManagerObject")->GetComponent<UI::BpmControlComponent>();
		auto loader                 = GetSceneObject<GameObject>("MusicObject")->GetComponent<Audio::LoadMusicComponent>();
		auto volume                 = GetSceneObject<GameObject>("MusicObject")->GetComponent<Audio::VolumeControlComponent>();
		auto playButtonAudioManager = playBtn->GetComponent<Audio::AudioManagerComponent>();

		//--------------------------------------------------------
		// ノーツタイプの変更ボタン
		//--------------------------------------------------------
		auto noteTypeButtonTap        = GetSceneObject<UI::NoteTypeButton>("NoteTypeTapButton");
		auto noteTypeButtonHoldStart  = GetSceneObject<UI::NoteTypeButton>("NoteTypeHoldStartButton");
		auto noteTypeButtonHoldEnd    = GetSceneObject<UI::NoteTypeButton>("NoteTypeHoldEndButton");
		auto noteTypeButtonSync       = GetSceneObject<UI::NoteTypeButton>("NoteTypeSyncButton");
		auto noteTypeButtonDelete     = GetSceneObject<UI::NoteTypeButton>("NoteTypeDeleteButton");
		auto imageNotesObj            = GetSceneObject<UI::ImageNotesObject>("ImageNotesObject");

		//--------------------------------------------------------
		//	描画準の設定
		//--------------------------------------------------------
		// 背景を最背面に
		background->SetDrawOrder(-100);

		//--------------------------------------------------------
		// グリッドオブジェクトにスクロール・ズーム・最大再生時間を設定
		//--------------------------------------------------------
		if (secGridObj)
		{
			secGridObj->SetScrollY(scroll ? scroll->GetScroll() : 0);
			secGridObj->SetZoom(zoom ? zoom->GetZoom() : ZOOM_DEFAULT);
			secGridObj->SetMaxPlayTime(loader ? loader->GetMaxPlayTime() : 0);
		}

		//--------------------------------------------------------
		// BPMグリッドの設定
		//--------------------------------------------------------
		if (bpmGridObj)
		{
			bpmGridObj->SetScrollY(scroll ? scroll->GetScroll() : 0);
			bpmGridObj->SetZoom(zoom ? zoom->GetZoom() : ZOOM_DEFAULT);
			bpmGridObj->SetMaxPlayTime(loader ? loader->GetMaxPlayTime() : 0);
			bpmGridObj->SetBpm(bpm ? bpm->GetBpm() : BPM_DEFAULT);
		}

		//--------------------------------------------------------
		// 縦ラインの設定
		//--------------------------------------------------------
		if (verticalLinesObj)
		{
			verticalLinesObj->SetLineCount(static_cast<int>(DRAW_LINE_NUMBER));
		}

		//--------------------------------------------------------
		// 再生ボタンに音源ハンドルを設定
		//--------------------------------------------------------
		if (playBtn && loader)
		{
			// ローダーに現在読み込まれている音源がある場合、既存の再生用ハンドルとビジュアライザーのハンドルをリセット
			int loaderHandle = loader->GetHandle();
			if (loaderHandle != 0)
			{
				// 再生ボタン側のハンドルを設定
				playBtn->SetHandle(loaderHandle);
				playBtn->SetVolume(volume ? volume->GetVolume() : 0);

				// ビジュアライザーの更新用ハンドル設定（存在する場合）
				auto sv = GetSceneObject<UI::SoundVisualizerObject>("SoundVisualizer");
				if (sv)
				{
					// 既存のハンドルをクリアしてから設定
					sv->SetSoftHandle(loader->GetSoftHandle());
				}
			}
			else
			{
				// ローダーにハンドルが無ければ再生ボタンをリセット
				playBtn->SetHandle(0);
			}
		}

		//--------------------------------------------------------
		// スクロールバーに最大スクロールY座標を設定
		//--------------------------------------------------------
		if (scroll)
		{
			int maxScrollY = 0;

			// 秒グリッドとBPMグリッドの両方から最大値を取得して大きい方を使用
			if (secGridObj)
			{
				maxScrollY = secGridObj->GetMaxScrollY();
			}

			if (bpmGridObj)
			{
				int bpmMaxScrollY = bpmGridObj->GetMaxScrollY();

				if (bpmMaxScrollY > maxScrollY)
				{
					maxScrollY = bpmMaxScrollY;
				}
			}

			scroll->SetContentHeight(maxScrollY + UI_OFFSET_Y);
			scroll->SetBarPosition(SCREEN_W - SCROLL_BAR_WIDTH, SCROLL_BAR_WIDTH);
		}

		//--------------------------------------------------------
		// NotePlacerObject に現在のスクロールとズームを渡す
		//--------------------------------------------------------
		if (notePlacer)
		{
			notePlacer->SetMusicHandle(loader->GetHandle());
			notePlacer->SetScrollY(scroll ? scroll->GetScroll() : 0.0f);
			notePlacer->SetZoom(zoom ? zoom->GetZoom() : ZOOM_DEFAULT);
			notePlacer->SetBpm(bpm ? bpm->GetBpm() : BPM_DEFAULT);
		}

		//--------------------------------------------------------
		// データ保存ボタンに NotePlacerObject のノーツデータを渡す
		//--------------------------------------------------------
		if (saveBtn && notePlacer)
		{
			saveBtn->SetNotePlacerObject(notePlacer->GetNotes());

			if (loader)
			{
				saveBtn->SetMusicFilePath(loader->GetLoadedFilePath());
			}
		}

		//--------------------------------------------------------
		// サウンドビジュアライザーオブジェクトにハンドルを渡す
		//--------------------------------------------------------
		if (visualizer && loader && loader->GetSoftHandle() != 0)
		{
			visualizer->SetSoftHandle(loader->GetSoftHandle());

			if (playBtn && playButtonAudioManager)
			{
				visualizer->SetSampleTime(playButtonAudioManager->GetSampleTime());
			}
		}

		//--------------------------------------------------------
		// 再生中ラインオブジェクトに再生位置と最大再生時間を渡す
		//--------------------------------------------------------
		if (playTimeLineObj && loader)
		{
			if (playButtonAudioManager)
			{
				playTimeLineObj->SetPlayTime(playButtonAudioManager->GetMsTime());
				playTimeLineObj->SetIsPlaying(playButtonAudioManager->GetPlayeMode());
			}

			playTimeLineObj->SetMaxPlayTime(loader->GetMaxPlayTime());
			playTimeLineObj->SetScrollY(scroll ? scroll->GetScroll() : 0);
			playTimeLineObj->SetZoom(zoom ? zoom->GetZoom() : ZOOM_DEFAULT);
		}

		//--------------------------------------------------------
		// ノーツタイプの変更ボタン処理
		//--------------------------------------------------------

		// タップノーツ
		if (noteTypeButtonTap->IsOn())
		{
			notePlacer->SetNoteType(UI::NOTE_TYPE::NOTE_TYPE_TAP);
			imageNotesObj->SetCurrentNoteType(UI::NOTE_TYPE::NOTE_TYPE_TAP);
		}

		// ホールドノーツ開始
		if (noteTypeButtonHoldStart->IsOn())
		{
			notePlacer->SetNoteType(UI::NOTE_TYPE::NOTE_TYPE_HOLD_START);
			imageNotesObj->SetCurrentNoteType(UI::NOTE_TYPE::NOTE_TYPE_HOLD_START);
		}

		// ホールドノーツ終了
		if (noteTypeButtonHoldEnd->IsOn())
		{
			notePlacer->SetNoteType(UI::NOTE_TYPE::NOTE_TYPE_HOLD_END);
			imageNotesObj->SetCurrentNoteType(UI::NOTE_TYPE::NOTE_TYPE_HOLD_END);
		}

		// 同時押しノーツ
		if (noteTypeButtonSync->IsOn())
		{
			notePlacer->SetNoteType(UI::NOTE_TYPE::NOTE_TYPE_SYNC);
			imageNotesObj->SetCurrentNoteType(UI::NOTE_TYPE::NOTE_TYPE_SYNC);
		}

		// ノーツ削除
		if (noteTypeButtonDelete->IsOn())
		{
			notePlacer->SetNoteType(UI::NOTE_TYPE::NOTE_TYPE_DELETE);
			imageNotesObj->SetCurrentNoteType(UI::NOTE_TYPE::NOTE_TYPE_DELETE);
		}

		//--------------------------------------------------------
		// 現在の再生位置を取得して NotePlacerObject に渡す
		//--------------------------------------------------------
		if (playButtonAudioManager)
		{
			int currentPlayPosition = playButtonAudioManager->GetMsTime();

			notePlacer->SetCurrentPlayPosition(currentPlayPosition);
		}

		//--------------------------------------------------------
		// BPM文字列
		//--------------------------------------------------------
		if (bpmString)
		{
			bpmString->SetParam(BPM_STR_BPM, bpm->GetBpm());
			bpmString->ApplyFormat();
		}

		//--------------------------------------------------------
		// 音量文字列
		//--------------------------------------------------------
		if (volumeString)
		{
			volumeString->SetParam(VOLUME_STR_VOLUME, volume->GetVolume());
			volumeString->ApplyFormat();
		}

		//--------------------------------------------------------
		// 再生時間文字列（整数演算で分・秒を算出）
		//--------------------------------------------------------
		if (playButtonAudioManager)
		{
			int msTime          = playButtonAudioManager->GetMsTime();
			int playTimeMinutes = msTime / MS_PER_MIN;
			int playTimeSeconds = (msTime / MS_PER_SEC) % 60;

			playTimeString->SetParam(PARAMETER_MINUTES, playTimeMinutes);
			playTimeString->SetParam(PARAMETER_SECONDS, playTimeSeconds);
			playTimeString->ApplyFormat();
		}

		//--------------------------------------------------------
		// 再生バーオブジェクトの設定とシーク処理
		//--------------------------------------------------------
		if (playBar)
		{
			playBar->SetMaxPlayTime(loader ? loader->GetMaxPlayTime() : 0);

			// 再生中はオーディオの再生位置で再生バーを更新する
			// 停止中はユーザが再生バーで変更した位置を保持するため上書きしない
			if (playButtonAudioManager && playButtonAudioManager->GetPlayeMode())
			{
				playBar->SetCurrentPlayTime(playButtonAudioManager->GetMsTime());
			}

			// シーク要求チェック
			if (playBar->IsSeekRequested())
			{
				int seekTime = playBar->GetRequestedSeekTime();

				playBtn->SetSeekPosition(seekTime);

				if (playButtonAudioManager)
				{
					playButtonAudioManager->Seek(seekTime);
				}

				playBar->ResetSeekRequest();
			}
		}

		//--------------------------------------------------------
		// 途中読み込みボタンの処理
		//--------------------------------------------------------
		if (loadProgressBtn && loadProgressBtn->IsLoadSuccess())
		{
			// 読み込んだノーツデータをNotePlacerに渡す（読み込んだ音源があればそちらに切り替え）
			if (notePlacer)
			{
				std::vector<std::shared_ptr<UI::NoteObject>> loadedNotes;
				std::string loadedMusicPath;
				if (loadProgressBtn->ConsumeLoadSuccess(loadedNotes, loadedMusicPath))
				{
					notePlacer->SetNotes(loadedNotes);

					// 読み込んだ音源パスがある場合は必ず切り替える
					if (!loadedMusicPath.empty() && loader)
					{
						// 現在の再生を停止してから新しいファイルをロード
						if (playButtonAudioManager)
						{
							playButtonAudioManager->HandleStopSound();
						}

						bool ok = loader->LoadFromFile(loadedMusicPath);
						if (ok)
						{
							// 再生ボタンへ即時反映
							if (playBtn)
							{
								playBtn->SetHandle(loader->GetHandle());
								playBtn->SetVolume(volume ? volume->GetVolume() : 0);
							}

							// ビジュアライザーハンドル更新
							auto sv = GetSceneObject<UI::SoundVisualizerObject>("SoundVisualizer");
							if (sv)
							{
								sv->SetSoftHandle(loader->GetSoftHandle());
								if (playButtonAudioManager)
								{
									sv->SetSampleTime(playButtonAudioManager->GetSampleTime());
								}
							}

							// 再生バー・再生ライン位置をリセット
							if (playBar)
							{
								playBar->SetCurrentPlayTime(0);
							}
							if (playTimeLineObj)
							{
								playTimeLineObj->SetPlayTime(0);
							}
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------------
	//! @brief シーン描画
	//-----------------------------------------------------------
	void GameScene::SceneDraw()
	{
		// 再生時間ラベル描画
		DrawStringToHandle(PLAY_TIME_STR_X - 15.0f, PLAY_TIME_STR_Y - 40.0f,
				           "再生時間", COLOR_WHITE,
			           Font::GetFont(Font::FontIndex::FONT_SIZE_40_INDEX));
	}

	//-----------------------------------------------------------
	//! @brief シーン終了
	//-----------------------------------------------------------
	void GameScene::SceneEnd()
	{
	}
}
