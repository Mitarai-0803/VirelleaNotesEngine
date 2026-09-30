//----------------------------------------------------------------------------
//! @file   GameConstants.h
//! @brief  ゲームのシステム全体で使用する定数定義
//! @detail 画面、時間、ズーム、BPM、ノーツなどの各種定数を定義します。
//----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------
// 再生ライン関連
//--------------------------------------------------------
constexpr int PLAY_LINE_X = 0;          //!< 再生ラインの開始X座標
constexpr int PLAY_LINE_STR_X = 10;     //!< 「再生中/停止中」ラベル配置X座標
constexpr int TIME_LABEL_OFFSET = 25;   //!< ラベルをラインからどれだけ下にずらすか

//--------------------------------------------------------
// 画面関連
//--------------------------------------------------------
constexpr int SCREEN_W = 1280;                   //!< 画面幅
constexpr int SCREEN_H = 720;                    //!< 画面高さ
constexpr float HALF = 0.5f;                     //!< 半分の値
constexpr float TWICE = 2.0f;                    //!< 2倍にする値
constexpr float HALF_SCREEN_W = SCREEN_W * HALF; //!< 画面幅の半分
constexpr float HALF_SCREEN_H = SCREEN_H * HALF; //!< 画面高さの半分

//--------------------------------------------------------
// BPM関連
//--------------------------------------------------------
constexpr int BPM_DEFAULT = 120;            //!< 初期BPM
constexpr int BPM_MAX = 300;                //!< 最大BPM
constexpr int BPM_MIN = 30;                 //!< 最小BPM
constexpr int BPM_STEP = 1;                 //!< BPM変化量
constexpr int BPM_STR_X = 10;               //!< BPM表示X座標
constexpr int BPM_STR_Y = 5;                //!< BPM表示Y座標
constexpr int BPM_KEY_HOLD_THRESHOLD = 30;  //!< BPMのホールド認識までのフレーム数
constexpr int DRAW_BPM_GRID_X = 390;        //!< BPMグリッド表示X座標
constexpr int DRAW_BPM_GRID_Y = 5;          //!< BPMグリッド表示Y座標
constexpr int DRAW_BPM_OFFSET_X = 10;       //!< BPM数表示オフセットX座標
constexpr int DRAW_BPM_OFFSET_Y = 5;        //!< BPM数表示オフセットY座標

//--------------------------------------------------------
// 時間表示関連
//--------------------------------------------------------
constexpr int TIME_STR_Y = 40;              //!< 現在再生時間Y座標
constexpr int MAX_TIME_STR_Y = 75;          //!< 最大再生時間Y座標
constexpr float PLAY_TIME_STR_X = 750.0f;   //!< 現在再生時間X座標
constexpr float PLAY_TIME_STR_Y = 50.0f;    //!< 現在再生時間Y座標

//--------------------------------------------------------
// ズーム関連
//--------------------------------------------------------
constexpr float ZOOM_DEFAULT = 50.0f;  //!< 初期ズーム
constexpr float ZOOM_MAX = 200.0f;     //!< 最大ズーム
constexpr float ZOOM_MIN = 50.0f;      //!< 最小ズーム
constexpr float ZOOM_STEP = 10.0f;     //!< ズーム変更ステップ

//--------------------------------------------------------
// 数学関係
//--------------------------------------------------------
constexpr float PI = 3.14159265358979323846f;  //!< 円周率

//--------------------------------------------------------
// 時間変換定数
//--------------------------------------------------------
constexpr int MS_PER_SEC = 1000;        //!< 1秒のミリ秒
constexpr int MS_PER_MIN = 60000;       //!< 1分のミリ秒
constexpr int FRAME_RATE = 60;          //!< フレームレート(FPS)
constexpr int SECONDS_PER_MIN = 60;     //!< 1分あたりの秒数

//--------------------------------------------------------
// ノーツ・ライン関連（譜面作成用）
//--------------------------------------------------------
constexpr int INVALID_NOTE_OFFSET_Y = 50;       //!< ノーツの配置ができないライン上オフセットY座標
constexpr int NOTE_LINE_Y = 100;                //!< ノーツ配置ラインのY座標
constexpr int NOTE_SNAP_INTERVAL = 30;          //!< ノーツ配置スナップ間隔
constexpr float NOTE_DELETE_RADIUS = 30.0f;     //!< ノーツの衝突半径
constexpr float NOTE_LINE_WIDTH = 30.0f;        //!< ノーツ間またはノーツ線間の太さ