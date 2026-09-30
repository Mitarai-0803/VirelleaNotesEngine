//----------------------------------------------------------------------------
//! @file   note_placer_object.cpp
//! @brief  ノーツ配置オブジェクトの実装
//! @detail ノーツの生成、スナップ、描画、CSV保存など譜面編集の主要処理を実装します。
//----------------------------------------------------------------------------
#include "NotePlacerObject.h"
#include "AudioConstants.h"
#include "GameConstants.h"
#include "InputManager.h"
#include "UIConstants.h"
#include "NoteObject.h"
#include "Colors.h"
#include <algorithm>
#include <DxLib.h>
#include <fstream>
#include <cmath>
#include <limits>

namespace UI
{
//--------------------------------------------------------
// 初期化処理
//--------------------------------------------------------
void NotePlacerObject::OnInit()
{
    // スクロール・ズーム・BPMの初期化
    scroll_y_ = 0.0f;
    zoom_ = ZOOM_DEFAULT;
    bpm_ = BPM_DEFAULT;

    // SYNCドラッグ状態の初期化
    pending_sync_active_ = false;
    pending_sync_dragging_ = false;
    pending_sync_move_first_ = false;
    pending_sync_y_ = 0.0f;
    pending_sync_first_lane_ = -1;
    pending_sync_first_.reset();
    pending_sync_second_.reset();

    // キャッシュ初期化
    lane_width_cache_ = static_cast<int>(DRAW_LINE_W);
    left_lane_start_cache_ = static_cast<int>(HALF_SCREEN_W - (lane_width_cache_ * 2)) + UI_OFFSET_X;
    half_lane_cache_ = lane_width_cache_ * 0.5f;
    for (int i = 0; i < 4; ++i)
    {
        lane_centers_cache_[i] = static_cast<float>(left_lane_start_cache_ + i * lane_width_cache_ + lane_width_cache_ / 2);
    }
    zoom_ratio_cache_ = (zoom_ > 0.0f ? (zoom_ / ZOOM_DEFAULT) : 1.0f);
    pick_radius_cache_ = static_cast<float>(NOTE_DELETE_RADIUS);
}

//--------------------------------------------------------
// 更新処理
//--------------------------------------------------------
void NotePlacerObject::OnUpdate()
{
    // 更新開始時に zoom/derive 依存のキャッシュを更新
    zoom_ratio_cache_ = (zoom_ > 0.0f ? (zoom_ / ZOOM_DEFAULT) : 1.0f);
    pick_radius_cache_ = static_cast<float>(NOTE_DELETE_RADIUS);

    // マウス入力の取得
    bool leftPressed = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
    bool rightPressed = (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0;

    // ノーツ配置・削除の判定
    if (music_handle_ != 0 && music_handle_ != SOUND_HANDLE_ERROR && InputManager::InputManager::PushMouseInput(MOUSE_INPUT_LEFT))
    {
        int mouseX = 0;
        int mouseY = 0;
        GetMousePoint(&mouseX, &mouseY);

        // 削除モードの場合
        if (note_type_ == UI::NOTE_TYPE::NOTE_TYPE_DELETE)
        {
            HandleDeleteMode(mouseX, mouseY);
        }
        else
        {
            // 譜面エリア内なら配置処理
            if (IsInScoreArea(mouseX, mouseY))
            {
                HandlePlaceMode(mouseX, mouseY);
            }
        }
    }

    // SYNCノーツのドラッグ開始判定
    if (!pending_sync_active_ && rightPressed)
    {
        int mouseX = 0;
        int mouseY = 0;
        GetMousePoint(&mouseX, &mouseY);

        float zoomRatio = zoom_ratio_cache_;
        float pickRadius = pick_radius_cache_;
        int hitIndex = -1;
        float bestDist = FLT_MAX;

        // SYNCノーツのヒットテスト
        for (size_t i = 0; i < note_objects_.size(); ++i)
        {
            const auto& note = note_objects_[i];
            if (note->GetType() != NOTE_TYPE::NOTE_TYPE_SYNC) continue;
            float sx = note->GetPosX() + UI_OFFSET_X;
            float sy = note->GetPosY() * zoomRatio - scroll_y_ + UI_OFFSET_Y;
            float dx = static_cast<float>(mouseX) - sx;
            float dy = static_cast<float>(mouseY) - sy;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < pickRadius && dist < bestDist)
            {
                bestDist = dist;
                hitIndex = static_cast<int>(i);
            }
        }
        // SYNCクラスタの抽出
        if (hitIndex >= 0)
        {
            float baseY = note_objects_[hitIndex]->GetPosY();
            float yThreshold = NOTE_SNAP_INTERVAL * 0.5f;
            std::vector<std::shared_ptr<NoteObject>> cluster;
            cluster.reserve(4);
            for (const auto& note : note_objects_)
            {
                if (note->GetType() == NOTE_TYPE::NOTE_TYPE_SYNC && std::abs(note->GetPosY() - baseY) <= yThreshold)
                {
                    cluster.push_back(note);
                }
            }
            // クラスタ内の端ノーツ判定
            if (cluster.size() >= 2)
            {
                auto minmaxX = std::minmax_element(cluster.begin(), cluster.end(), [](const std::shared_ptr<NoteObject>& a, const std::shared_ptr<NoteObject>& b)
                {
                    return a->GetPosX() < b->GetPosX();
                });
                auto leftNote = *minmaxX.first;
                auto rightNote = *minmaxX.second;
                auto hitNote = note_objects_[hitIndex];
                std::shared_ptr<NoteObject> first = nullptr;
                std::shared_ptr<NoteObject> second = nullptr;
                if (std::abs(hitNote->GetPosX() - leftNote->GetPosX()) <= std::abs(hitNote->GetPosX() - rightNote->GetPosX()))
                {
                    first = leftNote;
                    second = rightNote;
                }
                else
                {
                    first = rightNote;
                    second = leftNote;
                }
                // ドラッグ状態の開始
                pending_sync_active_ = true;
                pending_sync_dragging_ = true;
                pending_sync_y_ = baseY;
                pending_sync_first_lane_ = GetLaneIndexFromPosX(first->GetPosX());
                pending_sync_first_ = first;
                pending_sync_second_ = second;
            }
        }
    }

    // SYNCペア配置ドラッグ中の移動処理
    if (pending_sync_active_)
    {
        if (rightPressed)
        {
            int mouseX = 0;
            int mouseY = 0;
            GetMousePoint(&mouseX, &mouseY);

            pending_sync_dragging_ = true;

            // どちらのノーツを掴むか判定
            if (pending_sync_first_ && pending_sync_second_)
            {
                float zoomRatio = zoom_ratio_cache_;
                float screenY = pending_sync_y_ * zoomRatio - scroll_y_ + UI_OFFSET_Y;
                float firstScreenX = pending_sync_first_->GetPosX() + UI_OFFSET_X;
                float secondScreenX = pending_sync_second_->GetPosX() + UI_OFFSET_X;
                float d1 = std::hypot(static_cast<float>(mouseX) - firstScreenX, static_cast<float>(mouseY) - screenY);
                float d2 = std::hypot(static_cast<float>(mouseX) - secondScreenX, static_cast<float>(mouseY) - screenY);
                pending_sync_move_first_ = (d1 <= d2);
            }
            int snappedX = SnapToVerticalLine(mouseX);
            int targetLane = GetLaneIndexFromPosX(static_cast<float>(snappedX));
            if (pending_sync_second_ && pending_sync_first_)
            {
                int otherLane = pending_sync_move_first_ ? GetLaneIndexFromPosX(pending_sync_second_->GetPosX()) : GetLaneIndexFromPosX(pending_sync_first_->GetPosX());
                if (targetLane == otherLane)
                {
                    return;
                }
                float targetX = static_cast<float>(GetLaneCenterX(targetLane));
                float otherX = pending_sync_move_first_ ? pending_sync_second_->GetPosX() : pending_sync_first_->GetPosX();
                // 配置可能か判定
                if (!IsOccupied(targetX, pending_sync_y_) && !PointOnHoldConnection(targetX, pending_sync_y_))
                {
                    if (!CrossesHoldConnectionHorizontal(pending_sync_y_, min(targetX, otherX), max(targetX, otherX)))
                    {
                        if (pending_sync_move_first_)
                        {
                            pending_sync_first_->SetPosition(targetX, pending_sync_y_);
                            pending_sync_first_lane_ = targetLane;
                        }
                        else
                        {
                            pending_sync_second_->SetPosition(targetX, pending_sync_y_);
                        }
                        float x1w = min(pending_sync_first_->GetPosX(), pending_sync_second_->GetPosX());
                        float x2w = max(pending_sync_first_->GetPosX(), pending_sync_second_->GetPosX());
                        // 水平線上のノーツ削除
                        DeleteNotesOnHorizontalLine(pending_sync_y_, x1w, x2w, pending_sync_first_, pending_sync_second_);
                    }
                }
            }
        }
        else if (pending_sync_dragging_)
        {
            // ドラッグ終了時の状態リセット
            pending_sync_active_ = false;
            pending_sync_dragging_ = false;
            pending_sync_move_first_ = false;
            pending_sync_y_ = 0.0f;
            pending_sync_first_lane_ = -1;
            pending_sync_first_.reset();
            pending_sync_second_.reset();
        }
    }
}

//--------------------------------------------------------
// 描画処理
//--------------------------------------------------------
void NotePlacerObject::OnDraw()
{
    // ホールド接続線上のノーツクリーン
    CleanNotesOnAllHoldConnections();
    // ホールド接続線の描画
    DrawHoldConnections();
    // SYNC接続線の描画
    DrawSyncConnections();
    // ノーツ本体の描画
    for (auto& note : note_objects_) {
        note->OnDraw(scroll_y_, zoom_);
    }
}

//--------------------------------------------------------
// 設置モードのクリック処理
//--------------------------------------------------------
void NotePlacerObject::HandlePlaceMode(int mouseX, int mouseY)
{
    // 配置座標計算
    float posX = static_cast<float>(SnapToVerticalLine(mouseX));
    float zoomRatio = (zoom_ > 0.0f ? (zoom_ / ZOOM_DEFAULT) : 1.0f);
    float worldYDefaultPx = (static_cast<float>(mouseY) - UI_OFFSET_Y + scroll_y_) / zoomRatio;
    float snappedWorldY = static_cast<float>(SnapToBpmLine(worldYDefaultPx));
    int laneIndex = GetLaneIndexFromPosX(posX);
    bool hasOpenHold = HasOpenHoldInLaneBefore(laneIndex, snappedWorldY);
    // ホールド終点以外禁止判定
    if (hasOpenHold && note_type_ != UI::NOTE_TYPE::NOTE_TYPE_HOLD_END)
    {
        return;
    }
    // 始点がない場合終点禁止
    if (!hasOpenHold && note_type_ == UI::NOTE_TYPE::NOTE_TYPE_HOLD_END)
    {
        return;
    }
    // HOLD以外は縦線上禁止
    if (note_type_ != UI::NOTE_TYPE::NOTE_TYPE_HOLD_START && note_type_ != UI::NOTE_TYPE::NOTE_TYPE_HOLD_END)
    {
        if (PointOnHoldConnection(posX, snappedWorldY)) return;
    }
    // 配置可能ならノーツ生成
    if (!IsOccupied(posX, snappedWorldY))
    {
        int genTimeMs = CalcTimeMsFromY(snappedWorldY);
        auto note = std::make_shared<NoteObject>(note_type_, posX, snappedWorldY, genTimeMs);
        note_objects_.push_back(note);
        // HOLD終点直後の縦線上ノーツ削除
        if (note_type_ == UI::NOTE_TYPE::NOTE_TYPE_HOLD_END)
        {
            std::shared_ptr<NoteObject> start = FindOpenHoldStartInLane(laneIndex, snappedWorldY);
            if (start)
            {
                float y1 = min(start->GetPosY(), note->GetPosY());
                float y2 = max(start->GetPosY(), note->GetPosY());
                DeleteNotesOnVerticalLine(laneIndex, y1, y2, start, note);
            }
        }
    }
}

//--------------------------------------------------------
// 縦ラインスナップ
//--------------------------------------------------------
int NotePlacerObject::SnapToVerticalLine(int x)
{
    // レーン中央座標計算
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    int laneIndex = (x - leftLaneStart) / laneWidth;
    if (laneIndex < 0) laneIndex = 0;
    if (laneIndex > 3) laneIndex = 3;
    return leftLaneStart + laneIndex * laneWidth + laneWidth / 2;
}

//--------------------------------------------------------
// レーンインデックスからレーン中央X座標を取得
//--------------------------------------------------------
int NotePlacerObject::GetLaneCenterX(int laneIndex) const
{
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    if (laneIndex < 0) laneIndex = 0;
    if (laneIndex > 3) laneIndex = 3;
    return leftLaneStart + laneIndex * laneWidth + laneWidth / 2;
}

//--------------------------------------------------------
// BPMラインスナップ
//--------------------------------------------------------
int NotePlacerObject::SnapToBpmLine(float worldYDefaultPx)
{
    // BPMグリッドにスナップ
    if (bpm_ <= 0) return static_cast<int>(worldYDefaultPx);
    float intervalSec = 60.0f / static_cast<float>(bpm_);
    float intervalPixels = intervalSec * ZOOM_DEFAULT;
    int nearestIndex = static_cast<int>(std::round(worldYDefaultPx / intervalPixels));
    return static_cast<int>(nearestIndex * intervalPixels);
}

//--------------------------------------------------------
// 既存ノーツの占有判定
//--------------------------------------------------------
bool NotePlacerObject::IsOccupied(float posX, float posY) const
{
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    float halfLane = laneWidth * 0.5f;
    float timeThreshold = NOTE_SNAP_INTERVAL * 0.5f;
    // 既存ノーツとの重複判定
    for (const auto& note : note_objects_)
    {
        if (std::abs(note->GetPosX() - posX) <= halfLane)
        {
            if (std::abs(note->GetPosY() - posY) <= timeThreshold)
            {
                return true;
            }
        }
    }
    return false;
}

//--------------------------------------------------------
// BPMの設定
//--------------------------------------------------------
void NotePlacerObject::SetBpm(int bpm)
{
    // BPM値の更新と既存ノーツの再スナップ
    if (bpm <= 0) {
        bpm_ = bpm;
        return;
    }
    bpm_ = bpm;
    for (auto& note : note_objects_)
    {
        float worldY = note->GetPosY();
        int snapped = SnapToBpmLine(worldY);
        note->SetPosition(note->GetPosX(), static_cast<float>(snapped));
        int newTimeMs = CalcTimeMsFromY(static_cast<float>(snapped));
        note->SetTimeMs(newTimeMs);
    }
}

//--------------------------------------------------------
// ノーツの種類を設定
//--------------------------------------------------------
void NotePlacerObject::SetNoteType(int type)
{
    // ノーツタイプの範囲判定
    if (type > NOTE_TYPE::NOTE_TYPE_MIN && type < NOTE_TYPE::NOTE_TYPE_MAX)
    {
        note_type_ = type;
    }
}

//--------------------------------------------------------
// 音源ハンドルの設定
//--------------------------------------------------------
void NotePlacerObject::SetMusicHandle(int handle)
{
    music_handle_ = handle;
}

//--------------------------------------------------------
// ノーツタイプを取得
//--------------------------------------------------------
int NotePlacerObject::GetNoteType()
{
    return note_type_;
}

//--------------------------------------------------------
// X座標からレーンインデックスを算出
//--------------------------------------------------------
int NotePlacerObject::GetLaneIndexFromPosX(float posX) const
{
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    int idx = (static_cast<int>(posX) - leftLaneStart) / laneWidth;
    if (idx < 0) idx = 0;
    if (idx > 3) idx = 3;
    return idx;
}

//--------------------------------------------------------
// 同レーンで指定Yより前に未終了の長押しが存在するか判定
//--------------------------------------------------------
bool NotePlacerObject::HasOpenHoldInLaneBefore(int laneIndex, float posY) const
{
    // 直近の始点ノーツ探索
    bool hasStart = false;
    float startY = 0.0f;
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    float centerX = leftLaneStart + laneIndex * laneWidth + laneWidth * 0.5f;
    float halfLane = laneWidth * 0.5f;
    for (const auto& note : note_objects_)
    {
        if (std::abs(note->GetPosX() - centerX) <= halfLane)
        {
            if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START && note->GetPosY() < posY)
            {
                if (!hasStart || note->GetPosY() > startY)
                {
                    hasStart = true;
                    startY = note->GetPosY();
                }
            }
        }
    }
    // 終点ノーツ探索
    if (!hasStart)
    {
        return false;
    }
    for (const auto& note : note_objects_)
    {
        if (std::abs(note->GetPosX() - centerX) <= halfLane)
        {
            if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END)
            {
                if (note->GetPosY() > startY && note->GetPosY() <= posY)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

//--------------------------------------------------------
// 譜面エリア内かどうか判定
//--------------------------------------------------------
bool NotePlacerObject::IsInScoreArea(int x, int y) const
{
    // X座標範囲判定
    if (x < HALF_SCREEN_W - static_cast<int>(DRAW_LINE_W) * 2 + DRAW_LINE_OFFSET_W + UI_OFFSET_X)
    {
        return false;
    }
    if (x > HALF_SCREEN_W + static_cast<int>(DRAW_LINE_W) * 2 - DRAW_LINE_OFFSET_W + UI_OFFSET_X)
    {
        return false;
    }
    // Y座標範囲判定
    if (y < UI_OFFSET_Y + INVALID_NOTE_OFFSET_Y)
    {
        return false;
    }
    return true;
}

//--------------------------------------------------------
// 削除モードのクリック処理
//--------------------------------------------------------
void NotePlacerObject::HandleDeleteMode(int mouseX, int mouseY)
{
    // 最も近いノーツ探索
    float closestDistance = FLT_MAX;
    auto closestNoteIt = note_objects_.end();
    for (auto it = note_objects_.begin(); it != note_objects_.end(); ++it)
    {
        float noteScreenX = (*it)->GetPosX() + UI_OFFSET_X;
        float zoomRatio = (zoom_ > 0.0f ? (zoom_ / ZOOM_DEFAULT) : 1.0f);
        float noteScreenY = ((*it)->GetPosY() * zoomRatio) - scroll_y_ + UI_OFFSET_Y;
        float dx = static_cast<float>(mouseX) - noteScreenX;
        float dy = static_cast<float>(mouseY) - noteScreenY;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestNoteIt = it;
        }
    }
    // 一定距離以内なら削除
    if (closestDistance <= NOTE_DELETE_RADIUS && closestNoteIt != note_objects_.end())
    {
        note_objects_.erase(closestNoteIt);
    }
}

//--------------------------------------------------------
// 長押し始点と終点の接続線を描画
//--------------------------------------------------------
void NotePlacerObject::DrawHoldConnections() const
{
    float zoomRatio = (zoom_ > 0.0f ? (zoom_ / ZOOM_DEFAULT) : 1.0f);
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    float halfLane = laneWidth * 0.5f;
    // 各レーンごとに接続線描画
    for (int lane = 0; lane < 4; ++lane)
    {
        float centerX = leftLaneStart + lane * laneWidth + halfLane;
        struct Item { int type; float y; };
        std::vector<Item> items;
        items.reserve(note_objects_.size());
        for (const auto& note : note_objects_)
        {
            if (std::abs(note->GetPosX() - centerX) <= halfLane)
            {
                items.push_back({ note->GetType(), note->GetPosY() });
            }
        }
        if (items.empty())
        {
            continue;
        }
        std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.y < b.y; });
        bool haveStart = false;
        float startY = 0.0f;
        for (const auto& it : items)
        {
            if (!haveStart)
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_START)
                {
                    haveStart = true;
                    startY = it.y;
                }
            }
            else
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_END && it.y > startY)
                {
                    int x = static_cast<int>(centerX + UI_OFFSET_X);
                    int y1 = static_cast<int>(startY * zoomRatio - scroll_y_ + UI_OFFSET_Y);
                    int y2 = static_cast<int>(it.y * zoomRatio - scroll_y_ + UI_OFFSET_Y);
                    DrawLine(x, y1, x, y2, COLOR_GREEN, static_cast<int>(NOTE_LINE_WIDTH));
                    haveStart = false;
                }
            }
        }
    }
}

//--------------------------------------------------------
// 同時押しノーツの接続線を描画
//--------------------------------------------------------
void NotePlacerObject::DrawSyncConnections() const
{
    float zoomRatio = (zoom_ > 0.0f ? (zoom_ / ZOOM_DEFAULT) : 1.0f);
    float yThreshold = NOTE_SNAP_INTERVAL * 0.5f;
    struct SyncItem { float y; int lane; float x; };
    std::vector<SyncItem> syncs;
    syncs.reserve(note_objects_.size());
    // SYNCノーツのクラスタリング
    for (const auto& note : note_objects_)
    {
        if (note->GetType() == NOTE_TYPE::NOTE_TYPE_SYNC)
        {
            syncs.push_back(SyncItem{ note->GetPosY(), GetLaneIndexFromPosX(note->GetPosX()), note->GetPosX() });
        }
    }
    if (syncs.size() < 2)
    {
        return;
    }
    std::sort(syncs.begin(), syncs.end(), [](const SyncItem& a, const SyncItem& b)
    {
        if (a.y == b.y) return a.lane < b.lane;
        return a.y < b.y;
    });
    std::size_t i = 0;
    while (i < syncs.size())
    {
        float baseY = syncs[i].y;
        std::vector<float> xs;
        std::size_t j = i;
        while (j < syncs.size() && std::abs(syncs[j].y - baseY) <= yThreshold)
        {
            xs.push_back(syncs[j].x);
            ++j;
        }
        std::sort(xs.begin(), xs.end());
        for (std::size_t k = 0; k + 1 < xs.size(); ++k)
        {
            float x1w = xs[k];
            float x2w = xs[k + 1];
            // ホールド縦線との交差判定
            if (!CrossesHoldConnectionHorizontal(baseY, min(x1w, x2w), max(x1w, x2w)))
            {
                int y = static_cast<int>(baseY * zoomRatio - scroll_y_ + UI_OFFSET_Y);
                int x1 = static_cast<int>(x1w + UI_OFFSET_X);
                int x2 = static_cast<int>(x2w + UI_OFFSET_X);
                DrawLine(x1, y, x2, y, COLOR_CYAN, static_cast<int>(NOTE_LINE_WIDTH));
            }
        }
        i = j;
    }
}

//--------------------------------------------------------
// 指定位置が既存のホールド接続線（縦線）上か判定
//--------------------------------------------------------
bool NotePlacerObject::PointOnHoldConnection(float posX, float posY) const
{
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    float halfLane = laneWidth * 0.5f;
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    // 各レーンごとに区間判定
    for (int lane = 0; lane < 4; ++lane)
    {
        float centerX = leftLaneStart + lane * laneWidth + halfLane;
        struct Item { int type; float y; };
        std::vector<Item> items;
        for (const auto& note : note_objects_)
        {
            if (std::abs(note->GetPosX() - centerX) <= halfLane)
            {
                if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START || note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END)
                {
                    items.push_back({ note->GetType(), note->GetPosY() });
                }
            }
        }
        if (items.empty())
        {
            continue;
        }
        std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.y < b.y; });
        bool open = false;
        float startY = 0.0f;
        for (const auto& it : items)
        {
            if (!open)
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_START)
                {
                    open = true;
                    startY = it.y;
                }
            }
            else
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_END && it.y > startY)
                {
                    if (posY >= startY && posY <= it.y && std::abs(posX - centerX) <= halfLane * 0.4f)
                    {
                        return true;
                    }
                    open = false;
                }
            }
        }
    }
    return false;
}

//--------------------------------------------------------
// 指定Yで、x1-x2間がホールド接続線（縦線）に交差するか判定
//--------------------------------------------------------
bool NotePlacerObject::CrossesHoldConnectionHorizontal(float y, float x1, float x2) const
{
    if (x1 > x2) std::swap(x1, x2);
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    float halfLane = laneWidth * 0.5f;
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    // 各レーンごとに区間判定
    for (int lane = 0; lane < 4; ++lane)
    {
        float centerX = leftLaneStart + lane * laneWidth + halfLane;
        struct Item { int type; float y; };
        std::vector<Item> items;
        for (const auto& note : note_objects_)
        {
            if (std::abs(note->GetPosX() - centerX) <= halfLane)
            {
                if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START || note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END)
                {
                    items.push_back({ note->GetType(), note->GetPosY() });
                }
            }
        }
        if (items.empty()) continue;
        std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.y < b.y; });
        bool open = false;
        float startY = 0.0f;
        for (const auto& it : items)
        {
            if (!open)
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_START)
                {
                    open = true;
                    startY = it.y;
                }
            }
            else
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_END && it.y > startY)
                {
                    if (y >= startY && y <= it.y)
                    {
                        if (centerX >= x1 && centerX <= x2)
                        {
                            return true;
                        }
                    }
                    open = false;
                }
            }
        }
    }
    return false;
}

//--------------------------------------------------------
// 同一Yの水平方向接続上にあるノーツを削除
//--------------------------------------------------------
void NotePlacerObject::DeleteNotesOnHorizontalLine(float y, float x1, float x2,
    const std::shared_ptr<NoteObject>& e1,
    const std::shared_ptr<NoteObject>& e2)
{
    if (x1 > x2) std::swap(x1, x2);
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    float halfLane = laneWidth * 0.5f;
    float yThreshold = NOTE_SNAP_INTERVAL * 0.5f;
    // remove_ifで削除対象をまとめて消去
    note_objects_.erase(
        std::remove_if(note_objects_.begin(), note_objects_.end(),
            [&](const std::shared_ptr<NoteObject>& note) {
                if (note.get() == e1.get() || note.get() == e2.get()) return false;
                if (std::abs(note->GetPosY() - y) > yThreshold) return false;
                return (note->GetPosX() + halfLane >= x1 && note->GetPosX() - halfLane <= x2);
            }),
        note_objects_.end());
}

//--------------------------------------------------------
// 同一レーンの縦方向接続上にあるノーツを削除
//--------------------------------------------------------
void NotePlacerObject::DeleteNotesOnVerticalLine(int laneIndex, float y1, float y2,
    const std::shared_ptr<NoteObject>& s,
    const std::shared_ptr<NoteObject>& e)
{
    if (y1 > y2) std::swap(y1, y2);
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    float halfLane = laneWidth * 0.5f;
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    float centerX = leftLaneStart + laneIndex * laneWidth + halfLane;
    note_objects_.erase(
        std::remove_if(note_objects_.begin(), note_objects_.end(),
            [&](const std::shared_ptr<NoteObject>& note) {
                if (note.get() == s.get() || note.get() == e.get()) return false;
                if (std::abs(note->GetPosX() - centerX) > halfLane) return false;
                return (note->GetPosY() >= y1 && note->GetPosY() <= y2);
            }),
        note_objects_.end());
}

//--------------------------------------------------------
// 同レーンの未クローズな長押し始点ノーツを探す
//--------------------------------------------------------
std::shared_ptr<NoteObject> NotePlacerObject::FindOpenHoldStartInLane(int laneIndex, float endY) const
{
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    float halfLane = laneWidth * 0.5f;
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    float centerX = leftLaneStart + laneIndex * laneWidth + halfLane;
    bool hasStart = false;
    float startY = 0.0f;
    std::shared_ptr<NoteObject> startPtr = nullptr;
    // 始点ノーツ探索
    for (const auto& note : note_objects_)
    {
        if (std::abs(note->GetPosX() - centerX) <= halfLane)
        {
            if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START && note->GetPosY() < endY)
            {
                if (!hasStart || note->GetPosY() > startY)
                {
                    hasStart = true;
                    startY = note->GetPosY();
                    startPtr = note;
                }
            }
        }
    }
    // 終点ノーツ探索
    if (!hasStart)
    {
        return nullptr;
    }
    for (const auto& note : note_objects_)
    {
        if (std::abs(note->GetPosX() - centerX) <= halfLane)
        {
            if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END)
            {
                if (note->GetPosY() > startY && note->GetPosY() <= endY)
                {
                    return nullptr;
                }
            }
        }
    }
    return startPtr;
}

//--------------------------------------------------------
// 既存の全てのホールド接続線上にあるノーツを一括削除
//--------------------------------------------------------
void NotePlacerObject::CleanNotesOnAllHoldConnections()
{
    int laneWidth = static_cast<int>(DRAW_LINE_W);
    float halfLane = laneWidth * 0.5f;
    int leftLaneStart = static_cast<int>(HALF_SCREEN_W - (laneWidth * 2)) + UI_OFFSET_X;
    struct Segment { int lane; float x; float y1; float y2; };
    std::vector<Segment> segs;
    segs.reserve(16);
    // 各レーンごとに区間抽出
    for (int lane = 0; lane < 4; ++lane)
    {
        float centerX = leftLaneStart + lane * laneWidth + halfLane;
        struct Item { int type; float y; };
        std::vector<Item> items;
        for (const auto& note : note_objects_)
        {
            if (std::abs(note->GetPosX() - centerX) <= halfLane)
            {
                if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START || note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END)
                {
                    items.push_back({ note->GetType(), note->GetPosY() });
                }
            }
        }
        if (items.empty()) continue;
        std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.y < b.y; });
        bool open = false;
        float startY = 0.0f;
        for (const auto& it : items)
        {
            if (!open)
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_START)
                {
                    open = true;
                    startY = it.y;
                }
            }
            else
            {
                if (it.type == NOTE_TYPE::NOTE_TYPE_HOLD_END && it.y > startY)
                {
                    segs.push_back(Segment{ lane, centerX, startY, it.y });
                    open = false;
                }
            }
        }
    }
    if (segs.empty()) return;
    std::vector<std::size_t> removeIndices;
    removeIndices.reserve(note_objects_.size());
    // 削除対象ノーツ探索
    for (std::size_t i = 0; i < note_objects_.size(); ++i)
    {
        const auto& note = note_objects_[i];
        for (const auto& seg : segs)
        {
            if (std::abs(note->GetPosX() - seg.x) <= halfLane)
            {
                if (note->GetPosY() > seg.y1 && note->GetPosY() < seg.y2)
                {
                    if (note->GetType() != NOTE_TYPE::NOTE_TYPE_HOLD_START && note->GetType() != NOTE_TYPE::NOTE_TYPE_HOLD_END)
                    {
                        removeIndices.push_back(i);
                        break;
                    }
                }
            }
        }
    }
    // 後ろから削除
    for (std::size_t ri = removeIndices.size(); ri > 0; --ri)
    {
        note_objects_.erase(note_objects_.begin() + static_cast<std::ptrdiff_t>(removeIndices[ri - 1]));
    }
}

//--------------------------------------------------------
// 接続されていないノーツをノーマルノーツに変更
//--------------------------------------------------------
void NotePlacerObject::ConvertDisconnectedNotesToNormal()
{
    // 接続判定とノーマル化
    for (auto& note : note_objects_)
    {
        if (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START || note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END)
        {
            bool isConnected = false;
            for (const auto& otherNote : note_objects_)
            {
                if (note == otherNote)
                {
                    continue;
                }
                if (GetLaneIndexFromPosX(note->GetPosX()) == GetLaneIndexFromPosX(otherNote->GetPosX()))
                {
                    if ((note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START && otherNote->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END && otherNote->GetPosY() > note->GetPosY()) ||
                        (note->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_END && otherNote->GetType() == NOTE_TYPE::NOTE_TYPE_HOLD_START && otherNote->GetPosY() < note->GetPosY()))
                    {
                        isConnected = true;
                        break;
                    }
                }
            }
            if (!isConnected)
            {
                note->SetType(NOTE_TYPE::NOTE_TYPE_TAP);
            }
        }
    }
}

//--------------------------------------------------------
// 現在の再生位置を設定
//--------------------------------------------------------
void NotePlacerObject::SetCurrentPlayPosition(int position)
{
    current_play_position_ = position;
}

//--------------------------------------------------------
// 現在の再生位置を取得
//--------------------------------------------------------
int NotePlacerObject::GetCurrentPlayPosition() const
{
    return current_play_position_;
}

//--------------------------------------------------------
// Y座標から楽曲時間(ms)を計算
//--------------------------------------------------------
int NotePlacerObject::CalcTimeMsFromY(float worldYDefaultPx) const
{
    // Y座標から時間(ms)算出
    if (bpm_ <= 0)
    {
        float seconds = worldYDefaultPx / ZOOM_DEFAULT;
        return static_cast<int>(seconds * 1000.0f);
    }
    float secPerBeat = 60.0f / static_cast<float>(bpm_);
    float pixelsPerSec = ZOOM_DEFAULT;
    float seconds = worldYDefaultPx / pixelsPerSec;
    return static_cast<int>(seconds * 1000.0f);
}

}
