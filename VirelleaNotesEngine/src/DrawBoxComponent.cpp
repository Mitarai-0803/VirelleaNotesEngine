//----------------------------------------------------------------------------
//! @file   DrawBoxComponent.cpp
//! @brief  四角形描画コンポーネントの実装
//! @detail 四角形の描画、サイズや色の設定機能を提供します。
//----------------------------------------------------------------------------
#include "DrawBoxComponent.h"
#include "UIConstants.h"
#include "GameConstants.h"
#include <DxLib.h>

namespace UI
{
    //-----------------------------------------------------------
    //! コンストラクタ
    //! @detail メンバー変数を初期化
    //-----------------------------------------------------------
    DrawBoxComponent::DrawBoxComponent()
        : ComponentBase()
        , drawBox_{ 0 }
        , color_{ 0, 0, 0 }
    {
    }

    //-----------------------------------------------------------
    //! 初期化処理
    //-----------------------------------------------------------
    void DrawBoxComponent::Init()
    {
        drawBox_.x = 0;
        drawBox_.y = 0;
        drawBox_.w = SCREEN_W;
        drawBox_.h = UI_OFFSET_Y;

        SetColor(64, 64, 64);

        // 整数キャッシュ更新
        draw_x_int_ = static_cast<int>(drawBox_.x);
        draw_y_int_ = static_cast<int>(drawBox_.y);
        draw_w_int_ = static_cast<int>(drawBox_.w);
        draw_h_int_ = static_cast<int>(drawBox_.h);
    }

    //-----------------------------------------------------------
    //! 描画処理
    //-----------------------------------------------------------
    void DrawBoxComponent::Draw()
    {
        // 描画矩形
        DrawFillBox(draw_x_int_, draw_y_int_, draw_x_int_ + draw_w_int_, draw_y_int_ + draw_h_int_, color_handle_);
    }

    //-----------------------------------------------------------
    //! サイズを設定
    //! @param [in] x X座標
    //! @param [in] y Y座標
    //! @param [in] w 幅
    //! @param [in] h 高さ
    //-----------------------------------------------------------
    void DrawBoxComponent::SetBoxSize(float x, float y, float w, float h)
    {
        drawBox_.x = x;
        drawBox_.y = y;
        drawBox_.w = w;
        drawBox_.h = h;

        // 整数キャッシュ更新
        draw_x_int_ = static_cast<int>(drawBox_.x);
        draw_y_int_ = static_cast<int>(drawBox_.y);
        draw_w_int_ = static_cast<int>(drawBox_.w);
        draw_h_int_ = static_cast<int>(drawBox_.h);
    }

    //-----------------------------------------------------------
    //! 色を設定
    //! @param [in] r 赤
    //! @param [in] g 緑
    //! @param [in] b 青
    //-----------------------------------------------------------
    void DrawBoxComponent::SetColor(int r, int g, int b)
    {
        color_.r = r;
        color_.g = g;
        color_.b = b;

        // 色ハンドルをキャッシュ
        color_handle_ = GetColor(r, g, b);
    }
} // namespace UI
