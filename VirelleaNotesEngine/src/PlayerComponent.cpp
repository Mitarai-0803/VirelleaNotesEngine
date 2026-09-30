#include "PlayerComponent.h"
#include "DxLib.h"

//-----------------------------------------------------------
/// @brief 初期化処理
//-----------------------------------------------------------
void PlayerComponent::Init() {
    x = 300;  // 仮の判定ラインX座標
    y = 500;  // 判定ラインY座標
    hitBoxWidth = 64;
    hitBoxHeight = 64;
    score = 0;
    combo = 0;
    handle = LoadGraph("data/player.png"); // 画像読み込み
}

//-----------------------------------------------------------
/// @brief 毎フレーム更新処理
//-----------------------------------------------------------
void PlayerComponent::Update() {
    // キー入力で判定チェック（スペースキー例）
    if (CheckHitKey(KEY_INPUT_SPACE)) {
        // ノーツヒット判定（仮）
        bool hit = true; // 実際はノーツ位置と比較
        if (hit) {
            score += 100;
            combo++;
            // ヒットエフェクト呼び出し（EffectComponent経由など）
        }
        else {
            combo = 0;
        }
    }
}

//-----------------------------------------------------------
/// @brief 描画処理
//-----------------------------------------------------------
void PlayerComponent::Draw() {
    DrawGraph(x, y, handle, TRUE);
    // デバッグ: ヒットボックスを赤で描画
    DrawBox(x, y, x + hitBoxWidth, y + hitBoxHeight, GetColor(255, 0, 0), FALSE);
}

//-----------------------------------------------------------
/// @brief 終了処理
//-----------------------------------------------------------
void PlayerComponent::End() {
    DeleteGraph(handle);
}
