//----------------------------------------------------------------------------
//! @file   Colors.h
//! @brief  色定数の定義
//! @detail ウィンドウ背景色、透明色、基本色などの色定数を定義します。
//----------------------------------------------------------------------------
#pragma once
#include <DxLib.h>

//--------------------------------------------------------
// ウィンドウ背景色定数
//--------------------------------------------------------
constexpr int WINDOW_BG_COLOR_R = 100;    //!< 赤成分
constexpr int WINDOW_BG_COLOR_G = 100;    //!< 緑成分
constexpr int WINDOW_BG_COLOR_B = 100;    //!< 青成分

//--------------------------------------------------------
// トランスカラー定数
//--------------------------------------------------------
constexpr int TRANSPARENT_COLOR_R = 255;    //!< 赤成分
constexpr int TRANSPARENT_COLOR_G = 255;    //!< 緑成分
constexpr int TRANSPARENT_COLOR_B = 255;    //!< 青成分

//--------------------------------------------------------
// 基本色定数
//--------------------------------------------------------
const unsigned int COLOR_WHITE = GetColor(255, 255, 255);      //!< 白
const unsigned int COLOR_BLACK = GetColor(0, 0, 0);            //!< 黒
const unsigned int COLOR_RED = GetColor(255, 0, 0);            //!< 赤
const unsigned int COLOR_GREEN = GetColor(0, 255, 0);          //!< 緑
const unsigned int COLOR_BLUE = GetColor(0, 0, 255);           //!< 青
const unsigned int COLOR_YELLOW = GetColor(255, 255, 0);       //!< 黄色
const unsigned int COLOR_ORANGE = GetColor(255, 127, 80);      //!< オレンジ色
const unsigned int COLOR_CYAN = GetColor(0, 255, 255);         //!< 水色
const unsigned int COLOR_MAGENTA = GetColor(255, 0, 255);      //!< 紫
const unsigned int COLOR_GRAY = GetColor(128, 128, 128);       //!< グレー
const unsigned int COLOR_DARKGRAY = GetColor(64, 64, 64);      //!< 濃いグレー
const unsigned int COLOR_LIGHTGRAY = GetColor(192, 192, 192);  //!< 明るいグレー
