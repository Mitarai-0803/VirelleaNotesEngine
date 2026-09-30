//----------------------------------------------------------------------------
//! @file   MyLibraryFunctions.h
//! @brief  汎用ユーティリティの宣言
//! @detail 時間変換、当たり判定、角度変換、カメラ制御、ZIP作成などの共通関数を提供します。
//----------------------------------------------------------------------------
#pragma once
#include "Structs.h"
#include <string>
#include <vector>
#include <utility>

namespace MyLibrary
{
    //! ミリ秒を秒に変換します
    //! @param  [in] milli ミリ秒
    //! @return 秒
    float ConvertMilliToSec(int milli);

    //! ミリ秒を分に変換します
    //! @param  [in] milli ミリ秒
    //! @return 分
    float ConvertMilliToMinute(int milli);

    //! 秒数をフレーム数に変換する関数
    int SecondsToFrames(float seconds);

    //! ファイルパスの拡張子を取得します
    //! @param  [in] filepath ファイルパスのC文字列
    //! @return 拡張子文字列（先頭のドットなし、存在しない場合は空文字）
    std::string GetFileExtension(const char* filepath);

    //! 目標座標に向けた回転角を計算して更新します
    //! @param  [in] now_pos   現在位置
    //! @param  [in] goal_pos  目標位置
    //! @param  [in] dir       現在角度（ラジアン）
    //! @param  [in] rot_speed 1フレーム当たりの最大回転量（度）
    //! @return 更新後の角度（ラジアン）
    float ObjectPointToDirection(FloatPos now_pos, FloatPos goal_pos, float dir, float rot_speed);

    //! カメラの停止座標を計算します
    //! @param  [in]  pos_x  対象X座標
    //! @param  [in]  pos_y  対象Y座標
    //! @param  [in]  w      対象領域幅
    //! @param  [in]  h      対象領域高さ
    //! @param  [out] cam_x  計算結果のカメラX
    //! @param  [out] cam_y  計算結果のカメラY
    void CameraStop(float pos_x, float pos_y, float w, float h, float& cam_x, float& cam_y);

    //! 位置を矩形エリア内に収めます
    //! @param  [in,out] target_pos 対象位置
    //! @param  [in]     target_w   対象幅
    //! @param  [in]     target_h   対象高さ
    //! @param  [in]     area_w     エリア幅
    //! @param  [in]     area_h     エリア高さ
    void ClampPositionToArea(FloatPos& target_pos, int target_w, int target_h, float area_w, float area_h);

    //! 浮動小数点座標の点と矩形の当たり判定を行います
    //! @param  [in] point    点座標
    //! @param  [in] pos      矩形左上座標
    //! @param  [in] size     矩形サイズ
    //! @return 交差していればtrue
    bool CheckPointBoxHitF(const FloatPos& point, const FloatPos& pos, const FloatSize& size);

    //! 点と矩形の当たり判定を行います
    //! @param  [in] point_x 点X座標
    //! @param  [in] point_y 点Y座標
    //! @param  [in] box_x   矩形X座標
    //! @param  [in] box_y   矩形Y座標
    //! @param  [in] box_w   矩形幅
    //! @param  [in] box_h   矩形高さ
    //! @return 交差していればtrue
    bool CheckPointBoxHit(float point_x, float point_y, float box_x, float box_y, float box_w, float box_h);

    //! 矩形同士の当たり判定を行います
    //! @param  [in] x1 矩形1X座標
    //! @param  [in] y1 矩形1Y座標
    //! @param  [in] w1 矩形1幅
    //! @param  [in] h1 矩形1高さ
    //! @param  [in] x2 矩形2X座標
    //! @param  [in] y2 矩形2Y座標
    //! @param  [in] w2 矩形2幅
    //! @param  [in] h2 矩形2高さ
    //! @return 交差していればtrue
    bool CheckBoxHit(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

    //! 円同士の当たり判定を行います
    //! @param  [in] x1 円1X座標
    //! @param  [in] y1 円1Y座標
    //! @param  [in] r1 円1半径
    //! @param  [in] x2 円2X座標
    //! @param  [in] y2 円2Y座標
    //! @param  [in] r2 円2半径
    //! @return 交差していればtrue
    bool CheckCircleHit(float x1, float y1, float r1, float x2, float y2, float r2);

    //! 点と円の当たり判定を行います
    //! @param  [in] point_x 点X座標
    //! @param  [in] point_y 点Y座標
    //! @param  [in] circle_x 円X座標
    //! @param  [in] circle_y 円Y座標
    //! @param  [in] circle_r 円半径
    //! @return 交差していればtrue
    bool CheckPointCircleHit(float point_x, float point_y, float circle_x, float circle_y, float circle_r);

    //! 度数法をラジアンに変換します
    //! @param  [in] degree 角度（度）
    //! @return 角度（ラジアン）
    float TORADIAN(float degree);

    //! ラジアンを度数法に変換します
    //! @param  [in] radian 角度（ラジアン）
    //! @return 角度（度）
    float TODEGREE(float radian);

    //! 回転方向のX成分を取得します
    //! @param  [in] rot 角度（ラジアン）
    //! @return X成分
    float GetLengthAlongRotation(float rot);

    //! 回転方向のY成分を取得します
    //! @param  [in] rot 角度（ラジアン）
    //! @return Y成分
    float GetLengthOppositeRotation(float rot);

    //! 簡易ZIPを作成します
    //! @param  [in] zip_path      出力するzipファイルのフルパス
    //! @param  [in] src_and_names {元ファイルのフルパス, zip内でのファイル名} の配列
    //! @return 成功ならtrue、失敗ならfalse
    bool CreateZipStore(const std::string& zip_path,const std::vector<std::pair<std::string, std::string>>& src_and_names);
}
