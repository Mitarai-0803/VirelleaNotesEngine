//----------------------------------------------------------------------------
//! @file   NotePlacerObject.h
//! @brief  ノーツ配置オブジェクトの宣言
//! @detail ノーツの生成、スナップ、描画、CSV保存など譜面編集の主要処理を提供します。
//----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"
#include "NoteObject.h"
#include "ImageObject.h"
#include <vector>
#include <memory>
#include <array>

namespace UI
{
	//============================================================================
	//! @class  NotePlacerObject
	//! ノーツの編集操作と描画を管理するオブジェクト
	//============================================================================
	class NotePlacerObject : public GameObject
	{
	public:
		void OnInit() override;
		void OnUpdate() override;
		void OnDraw() override;

		void SetScrollY(float scrollY) { scroll_y_ = scrollY; }
		void SetZoom(float zoom) { zoom_ = zoom; }
		void SetBpm(int bpm);
		std::vector<std::shared_ptr<NoteObject>> GetNotes() const { return note_objects_; }
		void SetNoteType(int type);
		void SetMusicHandle(int handle);
		int GetNoteType();
		void ConvertDisconnectedNotesToNormal();
		void SetCurrentPlayPosition(int position);
		int GetCurrentPlayPosition() const;
		int CalcTimeMsFromY(float worldYDefaultPx) const;

		//-----------------------------------------------------------
		//! @brief ノーツデータを外部から設定
		//! @param [in] notes 設定するノーツデータ
		//-----------------------------------------------------------
		void SetNotes(const std::vector<std::shared_ptr<NoteObject>>& notes) { note_objects_ = notes; }

	private:
		int SnapToVerticalLine(int x);
		int SnapToBpmLine(float y);
		bool IsOccupied(float posX, float posY) const;
		int GetLaneIndexFromPosX(float posX) const;
		int GetLaneCenterX(int laneIndex) const;
		bool HasOpenHoldInLaneBefore(int laneIndex, float posY) const;
		bool IsInScoreArea(int x, int y) const;
		void HandleDeleteMode(int mouseX, int mouseY);
		void HandlePlaceMode(int mouseX, int mouseY);
		void DrawHoldConnections() const;
		void DrawSyncConnections() const;
		bool PointOnHoldConnection(float posX, float posY) const;
		bool CrossesHoldConnectionHorizontal(float y, float x1, float x2) const;
		void DeleteNotesOnHorizontalLine(float y, float x1, float x2,
		                                 const std::shared_ptr<NoteObject>& e1,
		                                 const std::shared_ptr<NoteObject>& e2);
		void DeleteNotesOnVerticalLine(int laneIndex, float y1, float y2,
		                               const std::shared_ptr<NoteObject>& s,
		                               const std::shared_ptr<NoteObject>& e);
		void CleanNotesOnAllHoldConnections();
		std::shared_ptr<NoteObject> FindOpenHoldStartInLane(int laneIndex, float endY) const;

		int music_handle_ = 0;                                      //!< 音源ハンドル
		int bpm_ = 0;                                               //!< 曲のBPM
		int note_type_ = 0;                                         //!< 現在のノーツタイプ
		float scroll_y_ = 0.0f;                                     //!< スクロールY値
		float zoom_ = 0.0f;                                         //!< ズーム値
		std::vector<std::shared_ptr<NoteObject>> note_objects_;     //!< ノーツ配列
		bool pending_sync_active_ = false;                          //!< SYNCドラッグがアクティブか
		bool pending_sync_dragging_ = false;                        //!< ドラッグ中フラグ
		bool pending_sync_move_first_ = false;                      //!< どちらを動かすか
		float pending_sync_y_ = 0.0f;                               //!< SYNCの基準Y
		int pending_sync_first_lane_ = -1;                          //!< SYNCの第一ノーツのレーン
		std::shared_ptr<NoteObject> pending_sync_first_;            //!< SYNC第一ノーツ
		std::shared_ptr<NoteObject> pending_sync_second_;           //!< SYNC第二ノーツ
		int current_play_position_ = 0;                             //!< 現在再生位置(ms)

		// キャッシュされた描画/計算用値
		int lane_width_cache_ = 0;                                 //!< レーン幅キャッシュ
		int left_lane_start_cache_ = 0;                            //!< レーン左始点Xキャッシュ
		float half_lane_cache_ = 0.0f;                             //!< レーン半幅キャッシュ
		float zoom_ratio_cache_ = 1.0f;                            //!< ズーム比率キャッシュ
		float pick_radius_cache_ = 0.0f;                           //!< ノーツピック半径キャッシュ
		std::array<float, 4> lane_centers_cache_ = { 0.0f, 0.0f, 0.0f, 0.0f }; //!< レーン中心Xキャッシュ
	};
}