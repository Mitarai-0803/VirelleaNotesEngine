//----------------------------------------------------------------------------
//! @file   UIConstants.h
//! @brief  UI関連定数の定義
//! @detail UI要素の配置、サイズ、パスなどの定数を定義します。
//----------------------------------------------------------------------------
#pragma once
#include "GameConstants.h"

//--------------------------------------------------------
// ボタン関係
//--------------------------------------------------------
constexpr int   BUTTON_NUMBER = 2;                                  //!< ボタンの数
constexpr int   BUTTON_Y = 50;                                      //!< ボタン配置Y座標
constexpr int   DATA_SAVE_BUTTON_X = 270;                           //!< データ保存ボタン配置Xオフセット(中心から)
constexpr float BUTTON_HOVER_SCALE = 1.1f;                          //!< ボタンホバー時のスケール倍率
constexpr float NOTES_TYPE_BTN_X = HALF_SCREEN_W + 320.0f;          //!< ノーツのタイプ変更ボタン配置X座標
constexpr float NOTES_TYPE_BTN_W_SPACE = 60.0f;                     //!< ノーツのタイプ変更ボタンの横の間隔
constexpr float NOTES_DELETE_BTN_X = NOTES_TYPE_BTN_X + 200.0f;		//!< ノーツの削除ボタン配置X座標
constexpr float NOTES_DELETE_BTN_Y = 50.0f;                         //!< ノーツの削除ボタン配置Y座標
constexpr float USAGE_DELETE_BTN_X = HALF_SCREEN_W - 380.0f;		//!< 使い方表示ボタン配置X座標
constexpr float USAGE_DELETE_BTN_Y = 50.0f;                         //!< 使い方表示ボタン配置Y座標
constexpr float LOAD_PROGRESS_BTN_X = HALF_SCREEN_W - 125.0f;		//!< 途中読み込みボタン配置X座標
constexpr float LOAD_PROGRESS_BTN_Y = 50.0f;                        //!< 途中読み込みボタン配置Y座標
constexpr float PLAY_BTN_X = HALF_SCREEN_W + 20.0f;                 //!< 再生停止ボタン配置X座標
constexpr float PLAY_BTN_Y = 50.0f;                                 //!< 再生停止ボタン配置Y座標

//--------------------------------------------------------
// スクロールバー関連
//--------------------------------------------------------
constexpr int SCROLL_BAR_WIDTH = 20;				//!< スクロールバーの幅
constexpr int SCROLL_BAR_MARGIN = 0;				//!< 右端からの余白(px)
constexpr int SCROLL_BAR_MIN_HEIGHT = 50;			//!< スクロールバーの最小高さ
constexpr int SCROLL_STEP = 20;						//!< スクロール量
constexpr int SCROLL_MIN_BASE = 0;					//!< スクロール最小値のベース
constexpr int SCROLL_MAX_BASE = SCROLL_STEP * 100;	//!< スクロール最大値のベース

//--------------------------------------------------------
// グリッド関連
//--------------------------------------------------------
constexpr int GRID_X_INTERVAL_SHORT = 300;                          //!< グリッド線X間隔(短い)
constexpr int GRID_X_INTERVAL_LONG = 330;                           //!< グリッド線X間隔(長い)
constexpr int DRAW_GRID_INTERVAL = 5;                               //!< グリッド線描画間隔
constexpr int GRID_LINE_OFFSET_X = 90;                              //!< グリッド線の秒数X表示オフセット
constexpr int GRID_LINE_OFFSET_Y = 15;                              //!< グリッド線の秒数Y表示オフセット
constexpr int EXPANDED_GRID_LINE_OFFSET_Y = GRID_LINE_OFFSET_Y * 2; //!< グリッド線の秒数Y表示オフセット2倍
constexpr float SCROLL_BAR_LINE_WIDTH = 5.0f;                       //!< グリッド線の太さ

//--------------------------------------------------------
// ライン関係
//--------------------------------------------------------
constexpr float DRAW_LINE_NUMBER = 4.0f;     //!< 描画するラインの数
constexpr float DRAW_LINE_W = 100.0f;        //!< 描画するラインの幅
constexpr float DRAW_LINE_OFFSET_W = 10.0f;  //!< 描画するラインの幅オフセット

//--------------------------------------------------------
// サウンドビジュアライザー表示関係
//--------------------------------------------------------
constexpr int  BUFFERLENGTH = 20;               //!< バッファ長
constexpr int  SOUNDLENGTH = 200;               //!< サウンド長
constexpr int  SOUND_VISUALOZER_OFFSET_Y = 10;  //!< オフセットY座標

//--------------------------------------------------------
// 再生バー関連
//--------------------------------------------------------
constexpr int PLAY_BAR_X = SCREEN_W - 100;  //!< 再生バーX座標
constexpr int PLAY_BAR_Y = 150;             //!< 再生バーY座標
constexpr int PLAY_BAR_WIDTH = 15;          //!< 再生バー幅
constexpr int PLAY_BAR_HEIGHT = 500;        //!< 再生バー高さ
constexpr int PLAY_BAR_RUDIUS = 15;         //!< 再生バーの円の半径

//--------------------------------------------------------
// 音量表示関係
//--------------------------------------------------------
constexpr int VOLUME_STR_X = 10;  //!< 音量表示X座標
constexpr int VOLUME_STR_Y = 50;  //!< 音量表示Y座標

//--------------------------------------------------------
// 読み込みメッセージ関係
//--------------------------------------------------------
constexpr char NORMAL_LOAD_MSG[] = "ドラッグアンドドロップで音源を読み込み";
constexpr char ERROR_LOAD_MSG[] = "読み込めないファイル形式です";
constexpr char RELOAD_MSG_SUFFIX[] = "秒後に読み込めるようになります...";

// 音源対応拡張子
constexpr char EXT_MP3[] = "mp3";
constexpr char EXT_WAV[] = "wav";
constexpr char EXT_OGG[] = "ogg";

//--------------------------------------------------------
// 背景関係
//--------------------------------------------------------
constexpr float	BG_SCROLL_SPEED = 0.1f;  //!< 背景スクロール速度

//--------------------------------------------------------
// UI表示関係
//--------------------------------------------------------
constexpr int UI_OFFSET_X = 0;    //!< UIオフセットX座標
constexpr int UI_OFFSET_Y = 100;  //!< UIオフセットY座標

//--------------------------------------------------------
// 画像表示関係
//--------------------------------------------------------
constexpr float IMAGE_OFFSET_X = 25.0f;  //!< 画像オフセットX座標
constexpr float IMAGE_OFFSET_Y = 25.0f;  //!< 画像オフセットY座標

//--------------------------------------------------------
// 文字列表示関係
//--------------------------------------------------------
constexpr const char* BPM_STR = "BPM:{bpm:%d}";                      //!< BPM表示文字列
constexpr const char* BPM_STR_BPM = "bpm";                           //!< BPMパラメータ名
constexpr const char* VOLUME_STR = "音量:{volume:%d}";               //!< 音量表示文字列
constexpr const char* VOLUME_STR_VOLUME = "volume";                  //!< 音量パラメータ名
constexpr const char* TIME_STR = "{minutes:%02d}:{seconds:%02d}";	 //!< 現在時間表示文字列
constexpr const char* TIME_STR_W = "%02d:%02d";                      //!< 現在時間表示文字列(幅取得用)
constexpr const char* PARAMETER_MINUTES = "minutes";                 //!< 分パラメータ名
constexpr const char* PARAMETER_SECONDS = "seconds";                 //!< 秒パラメータ名

//--------------------------------------------------------
// 画像パス
//--------------------------------------------------------
constexpr const char* BACKGROUND_IMAGE_PATH = "EngineData/Image/back.png";				   //!< 背景画像パス
constexpr const char* PLAY_BUTTON_PATH = "EngineData/Image/UI/play.png";				   //!< 再生ボタン画像パス
constexpr const char* STOP_BUTTON_PATH = "EngineData/Image/UI/stop.png";				   //!< 停止ボタン画像パス
constexpr const char* SAVE_BUTTON_PATH = "EngineData/Image/UI/save.png";				   //!< データ保存ボタン画像パス
constexpr const char* TAP_PATH = "EngineData/Image/UI/Notes/note (1).png";				   //!< タップノーツ画像パス
constexpr const char* HOLD_START_PATH = "EngineData/Image/UI/Notes/note (2).png";		   //!< ホールド(始まり)ノーツ画像パス
constexpr const char* HOLD_END_PATH = "EngineData/Image/UI/Notes/note (3).png";			   //!< ホールド(終わり)ノーツ画像パス
constexpr const char* SYNC_PATH = "EngineData/Image/UI/Notes/note (4).png";				   //!< 同時押しノーツ画像パス
constexpr const char* TAP_BUTTON_PATH = "EngineData/Image/UI/note_type (1).png";           //!< タップノーツ選択ボタン画像パス
constexpr const char* HOLD_START_BUTTON_PATH = "EngineData/Image/UI/note_type (2).png";    //!< ホールド(始まり)ノーツ選択ボタン画像パス
constexpr const char* HOLD_END_BUTTON_PATH = "EngineData/Image/UI/note_type (3).png";      //!< ホールド(終わり)ノーツ選択ボタン画像パス
constexpr const char* SYNC_BUTTON_PATH = "EngineData/Image/UI/note_type (4).png";          //!< 同時押しノーツ選択ボタン画像パス
constexpr const char* USAGE_BUTTON_PATH = "EngineData/Image/UI/help.png";                  //!< 使い方ボタン画像パス
constexpr const char* USAGE_BUTTON_DOWN_PATH = "EngineData/Image/UI/help_down.png";        //!< 使い方ボタン画像パス(押された時)
constexpr const char* DELETE_BUTTON_PATH = "EngineData/Image/UI/delete.png";               //!< ノーツ削除ボタン画像パス
constexpr const char* DELETE_CURSOR_PATH = "EngineData/Image/UI/MouseCursor/delete.png";   //!< ノーツ削除カーソル画像パス
constexpr const char* LOAD_PROGRESS_BUTTON_PATH = "EngineData/Image/UI/load.png";          //!< 途中読み込みボタン画像パス
constexpr const char* USAGE_PAGE_PATH = "EngineData/Image/usage_page_%d.png";              //!< 使い方ページの画像パス
constexpr const int   USAGE_PAGE_MAX = 2;                                                  //!< 使い方ページの最大番号

//--------------------------------------------------------
// 保存完了メッセージ関連
//--------------------------------------------------------
constexpr int SAVE_MESSAGE_DISPLAY_TIME = 60;  //!< 保存完了メッセージの表示時間(フレーム数)
